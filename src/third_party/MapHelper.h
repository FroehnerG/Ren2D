#pragma once
#ifndef MAPHELPER_H
#define MAPHELPER_H

/* MapHelper.h : Utility header containing convenient data structures for homework #3 */
/* (mainly so that you don't need to type or copy-paste directly from the spec) */

#include <string>
#include <vector>
#include "glm/glm.hpp"

struct Actor
{
public:
	std::string actor_name;
	char view;
	glm::ivec2 position;
	glm::ivec2 velocity;
	bool blocking;
	std::string nearby_dialogue;
	std::string contact_dialogue;

	Actor(std::string actor_name, char view, glm::ivec2 position, glm::ivec2 initial_velocity,
		bool blocking, std::string nearby_dialogue, std::string contact_dialogue)
		: actor_name(actor_name), view(view), position(position), velocity(initial_velocity), blocking(blocking), nearby_dialogue(nearby_dialogue), contact_dialogue(contact_dialogue) {
	}

	Actor() {}
};

const std::string initial_render = "\
       s     \n\
bbbb    s    \n\
f  bbbbb s   \n\
         b   \n\
      p  b   \n\
         b   \n\
bbbbbbbbb    \n\
             \n\
          b  ";


const int HARDCODED_MAP_WIDTH = 50;
const int HARDCODED_MAP_HEIGHT = 26;
const char hardcoded_map[HARDCODED_MAP_HEIGHT][HARDCODED_MAP_WIDTH + 1] = { // Know why we need "+1"? Hint : These are c-style strings.
	"bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
	"bb    b       b                   b        b    bb",
	"bb            b                   b             bb",
	"bb    bbbbbbbbbbbbbb              bbbbbbbbbb    bb",
	"bb            b      b            b             bb",
	"bb    b       b       b   bb      b        b    bb",
	"bb    b  bb bbb     bb      bb    bbbbbbbbbb    bb",
	"bb    b  b    b   bb          bb           b    bb",
	"bb    b  b   b  bb        bb    bb         b    bb",
	"bbb   bbbb  bbbb                  bbbbbbbbbb   bbb",
	"b                   bb                           b",
	"b                                                b",
	"bbb    bbbb  bbbb          bb    bbbbbbbbbbbb  bbb",
	"bb       b      bbbbb        bbb           b    bb",
	"bb       b            b    b               b    bb",
	"bb       b            b    b               b    bb",
	"bb       b            b    b               b    bb",
	"bb       bbbbbbbbbbbbb      bbbbbbbbbbbbbbbb    bb",
	"bb                                              bb",
	"bb        bb           b  b         bb          bb",
	"bb                    b    b            bb      bb",
	"bbbbb    bb           b    b                   bbb",
	"bb   bb                bbbb      bb      bb      b",
	"bb     bb    bb                      bb          b",
	"bbbbbbbbbbbbbbbb   bbbbbbbbbbbb   bbbbbbbbbbbbbbbb",
	"bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"
};

inline std::vector<Actor> hardcoded_actors = {
	Actor(
		"spartan1", // Actor name
		's', // view
		glm::ivec2(21, 12), // starting position
		glm::ivec2(0, 0), // starting velocity
		true, // blocking?
		"Hey, you! (the spartan throws a fake plastic spear at you. It bounces off but kinda hurts your ego a bit. [health down])", // nearby_dialogue
		"Gotcha! (the spartan is right next to you, and pokes you with a fake plastic spear. They're really into it and having a good time, and we can't be having that. [health down])" // contact_dialogue
	),
	Actor(
		"spartan2",
		's',
		glm::ivec2(22, 13),
		glm::ivec2(0, 0),
		true,
		"Hey, you! (the spartan throws a fake plastic spear at you. It bounces off but kinda hurts your ego a bit. [health down])",
		"Gotcha! (the spartan is right next to you, and pokes you with a fake plastic spear. They're really into it and having a good time, and we can't be having that. [health down])"
	),
	Actor(
		"spartan3",
		's',
		glm::ivec2(20, 11),
		glm::ivec2(0, 0),
		true,
		"Hey, you! (the spartan throws a fake plastic spear at you. It bounces off but kinda hurts your ego a bit. [health down])",
		"Gotcha! (the spartan is right next to you, and pokes you with a fake plastic spear. They're really into it and having a good time, and we can't be having that. [health down])"
	),
	Actor(
		"spartan4",
		's',
		glm::ivec2(18, 9),
		glm::ivec2(0, 0),
		true,
		"Hey, you! (the spartan throws a fake plastic spear at you. It bounces off but kinda hurts your ego a bit. [health down])",
		"Gotcha! (the spartan is right next to you, and pokes you with a fake plastic spear. They're really into it and having a good time, and we can't be having that. [health down])"
	),
	Actor(
		"spartan5",
		's',
		glm::ivec2(18, 10),
		glm::ivec2(-1, 0),
		true,
		"Hey, you! (the spartan throws a fake plastic spear at you. It bounces off but kinda hurts your ego a bit. [health down])",
		"Gotcha! (the spartan is right next to you, and pokes you with a fake plastic spear. They're really into it and having a good time, and we can't be having that. [health down])"
	),
	Actor(
		"spartan6",
		's',
		glm::ivec2(4, 11),
		glm::ivec2(1, 0),
		true,
		"Hey, you! (the spartan throws a fake plastic spear at you. It bounces off but kinda hurts your ego a bit. [health down])",
		"Gotcha! (the spartan is right next to you, and pokes you with a fake plastic spear. They're really into it and having a good time, and we can't be having that. [health down])"
	),
	Actor(
		"sparty",
		'S',
		glm::ivec2(12, 22),
		glm::ivec2(1, 0),
		false,
		"Oh no... it's Sparty himself! His sinister unflinching eyes scan the darkness in your direction. You'd better be extra careful!)",
		"(you bump into sparty and he falls onto you clumsily. It's all very embarrassing and you can feel the life squeezing out of you) [game over]"
	),
	Actor(
		"wolverine1",
		'w',
		glm::ivec2(11, 15),
		glm::ivec2(0, 0),
		false,
		"(One of your wolverine classmates is nearby) \"Hey-- be careful! Some spartans are sneaking around over there. Stay far away and slip past if you can... It looks like some of the other students are panicking, which I told them to not do! And now they're lost in the darkness. Look for their ('f') icon and go see if you can calm them down. Then, get out of there and go find Professor Yarger!\"",
		"(you're standing on your friend, but they don't seem to mind. They are a good friend.)"
	),
		Actor(
		"494 student",
		'4',
		glm::ivec2(10, 16),
		glm::ivec2(0, 0),
		true,
		"(There's an EECS 494 student working on her laptop nearby. She looks very busy and is doesn't want to talk.)",
		"()"
	),
	Actor(
		"exit_door_2",
		'd',
		glm::ivec2(48, 22),
		glm::ivec2(0, 0),
		false,
		"(An exit door is nearby)",
		"(you open the door) [you win]"
	),
	Actor(
		"exit_door_1",
		'd',
		glm::ivec2(48, 23),
		glm::ivec2(0, 0),
		false,
		"(An exit door is nearby)",
		"(you open the door) [you win]"
	),
	Actor(
		"friend1",
		'f',
		glm::ivec2(11, 8),
		glm::ivec2(0, 0),
		true,
		"H-hey (you spot a nearby friend in the darkness). Thank you for f-finding me. I'll sit here and rest a bit. [score up]",
		""
	),
	Actor(
		"friend2",
		'f',
		glm::ivec2(13, 13),
		glm::ivec2(0, 0),
		true,
		"O-oh (you spot a nearby friend in the darkness). I'm glad I'm not a-alone... I'll sit here and r-rest a bit. Thanks. [score up]",
		""
	),
	Actor(					/* This player actor is always processed last, as it is the last actor in the actor list. */
		"player",			// actor name
		'p',				// view
		glm::ivec2(19, 15),	// starting position
		glm::ivec2(0, 0),	// starting velocity
		false,				// blocking?
		"",					// nearby dialogue
		""					// made-contact dialogue
	),
};

#endif