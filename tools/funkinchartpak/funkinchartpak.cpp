/*
 * funkinchartpak by Regan "CuckyDev" Green
 * Packs Friday Night Funkin' json formatted legacy charts into a binary file for the PSX port
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <unordered_set>
#include <filesystem>

#include "json.hpp"
using json = nlohmann::json;

#define SECTION_FLAG_OPPFOCUS (1 << 15) //Focus on opponent
#define SECTION_FLAG_BPM_MASK 0x7FFF //1/24

struct Section
{
	uint16_t end;
	uint16_t flag = 0;
};

#define NOTE_FLAG_SUSTAIN     (1 << 2) //Note is a sustain note
#define NOTE_FLAG_SUSTAIN_END (1 << 3) //Is either end of sustain
#define NOTE_FLAG_ALT_ANIM    (1 << 4) //Note plays alt animation
#define NOTE_FLAG_HIT         (1 << 5) //Note has been hit

struct Note
{
	uint16_t pos; //1/12 steps
	uint16_t type, pad = 0;
};

typedef int32_t fixed_t;

#define FIXED_SHIFT (10)
#define FIXED_UNIT  (1 << FIXED_SHIFT)

uint16_t PosRound(double pos, double crochet)
{
	return (uint16_t)std::floor(pos / crochet + 0.5);
}

void WriteWord(std::ostream &out, uint16_t word)
{
	out.put(word >> 0);
	out.put(word >> 8);
}

void WriteLong(std::ostream &out, uint32_t word)
{
	out.put(word >> 0);
	out.put(word >> 8);
	out.put(word >> 16);
	out.put(word >> 24);
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		std::cout << "usage: funkinchartpak in_json" << std::endl;
		return 0;
	}
	
	//Read json
	std::ifstream i(argv[1]);
	if (!i.is_open())
	{
		std::cout << "Failed to open " << argv[1] << std::endl;
		return 1;
	}
	json j;
	i >> j;
	
	auto song_info = j["song"];
	
	double bpm = song_info["bpm"];
	double crochet = (60.0 / bpm) * 1000.0;
	double step_crochet = crochet / 4;
	
	double speed = song_info["speed"];
	
	std::cout << argv[1] << " speed: " << speed << " ini bpm: " << bpm << " step_crochet: " << step_crochet << std::endl;
	
	double milli_base = 0;
	uint16_t step_base = 0;
	
	std::vector<Section> sections;
	std::vector<Note> notes_0;
	std::vector<Note> notes_1;
	
	uint16_t section_end = 0;
	int score = 0, dups = 0;
	std::unordered_set<uint32_t> note_fudge;
	for (auto &i : song_info["notes"]) //Iterate through sections
	{
		bool is_opponent = i["mustHitSection"] != true; //Note: swapped
		
		//Read section
		Section new_section;
		if (i["changeBPM"] == true)
		{
			//Update BPM (THIS IS HELL!)
			milli_base += step_crochet * (section_end - step_base);
			step_base = section_end;
			
			bpm = i["bpm"];
			crochet = (60.0 / bpm) * 1000.0;
			step_crochet = crochet / 4;
			
			std::cout << "chg bpm: " << bpm << " step_crochet: " << step_crochet << " milli_base: " << milli_base << " step_base: " << step_base << std::endl;
		}
		new_section.end = (section_end += 16) * 12; //(uint16_t)i["lengthInSteps"]) * 12; //I had to do this for compatibility
		new_section.flag = PosRound(bpm, 1.0 / 24.0) & SECTION_FLAG_BPM_MASK; 
		bool is_alt = i["altAnim"] == true;
		if (is_opponent)
			new_section.flag |= SECTION_FLAG_OPPFOCUS;
		sections.push_back(new_section);
		
		//Read notes
		for (auto &j : i["sectionNotes"])
		{
			//Push main note
			Note new_note;
			
			new_note.pos = (step_base * 12) + PosRound(((double)j[0] - milli_base) * 12.0, step_crochet);
			new_note.type = (uint8_t)j[1] % 4;
			int sustain = (int)PosRound(j[2], step_crochet) - 1;
			
			if (sustain >= 0)
				new_note.type |= NOTE_FLAG_SUSTAIN_END;
			
			if (note_fudge.count(*((uint32_t*)&new_note))) {
				dups += 1;
				continue;
			}
			note_fudge.insert(*((uint32_t*)&new_note));
			
			if(((j[1] < 4) and !is_opponent) or (!(j[1] < 4) and is_opponent)) {
				notes_0.push_back(new_note);
				score += 350;
			}
			else
				notes_1.push_back(new_note);
			
			//Push sustain notes
			for (int k = 0; k <= sustain; k++)
			{
				Note sus_note;
				sus_note.pos = new_note.pos + ((k + 1) * 12);
				sus_note.type = new_note.type | NOTE_FLAG_SUSTAIN;
				if (k != sustain)
					sus_note.type &= ~NOTE_FLAG_SUSTAIN_END;
				
				if(((j[1] < 4) and !is_opponent) or (!(j[1] < 4) and is_opponent))
					notes_0.push_back(sus_note);
				else
					notes_1.push_back(sus_note);
			}
		}
	}
	std::cout << "max score: " << score << " dups excluded: " << dups << std::endl;
	
	//Sort notes
	std::sort(notes_0.begin(), notes_0.end(), [](Note a, Note b) {
		if (a.pos == b.pos)
			return (b.type & NOTE_FLAG_SUSTAIN) && !(a.type & NOTE_FLAG_SUSTAIN);
		else
			return a.pos < b.pos;
	});
	std::sort(notes_1.begin(), notes_1.end(), [](Note a, Note b) {
		if (a.pos == b.pos)
			return (b.type & NOTE_FLAG_SUSTAIN) && !(a.type & NOTE_FLAG_SUSTAIN);
		else
			return a.pos < b.pos;
	});
	
	//Push dummy section and note
	Section dum_section;
	dum_section.end = 0xFFFF;
	dum_section.flag = sections[sections.size() - 1].flag;
	sections.push_back(dum_section);
	
	Note dum_note;
	dum_note.pos = 0xFFFF;
	dum_note.type = NOTE_FLAG_HIT;
	
	notes_0.push_back(dum_note);
	notes_1.push_back(dum_note);
	
    // Write to output
    std::filesystem::path outputPath = std::filesystem::path(argv[1]).replace_extension(".cht");
    std::ofstream out(outputPath.string(), std::ostream::binary);
    
    if (!out.is_open()) {
        std::cout << "Failed to open " << outputPath << std::endl;
        return 1;
    }
	
	// Write header
	WriteLong(out, (fixed_t)(speed * FIXED_UNIT));
	WriteWord(out, 8 + (sections.size() << 2));  // sections offset
	WriteWord(out, notes_0.size() << 2);  // player 1 notes offset

	// Write sections
	for (auto &i : sections)
	{
		WriteWord(out, i.end);
		WriteWord(out, i.flag);
	}

	// Write Player 1 notes
	for (auto &i : notes_0)
	{
		WriteWord(out, i.pos);
		out.put(i.type);
		out.put(0);
	}

	// Write Player 2 notes (empty if not used)
	for (auto &i : notes_1)
	{
		WriteWord(out, i.pos);
		out.put(i.type);
		out.put(0);
	}
	return 0;
}