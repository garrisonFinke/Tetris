#pragma once

#include <windows.h>
#include <string>
#include <vector>

inline const std::vector<std::string> tPiece = {
	"     TTT  T     ",
	"  T  TT   T     ",
	"  T  TTT        ",
	"  T   TT  T     "
};

inline const std::vector<std::string> jPiece = {
	"     JJJ   J    ",
	"  J   J  JJ     ",
	" J   JJJ        ",
	"  JJ  J   J     "
};

inline const std::vector<std::string> zPiece = {
	"     ZZ   ZZ    ",
	"       Z  ZZ  Z ",
	"     ZZ   ZZ    ",
	"       Z  ZZ  Z "
};

inline const std::vector<std::string> oPiece = {
	"     OO  OO     ",
	"     OO  OO     ",
	"     OO  OO     ",
	"     OO  OO     "
};

inline const std::vector<std::string> sPiece = {
	"      SS SS     ",
	"      S   SS   S",
	"      SS SS     ",
	"      S   SS   S"
};

inline const std::vector<std::string> lPiece = {
	"     LLL L      ",
	" LL   L   L     ",
	"   L LLL        ",
	"  L   L   LL    ",
};

inline const std::vector<std::string> iPiece = {
	"    IIII        ",
	"  I   I   I   I ",
	"    IIII        ",
	"  I   I   I   I "
};

inline const std::vector<std::vector<std::string>> tetrominos = {
	tPiece,
	jPiece,
	zPiece,
	oPiece,
	sPiece,
	lPiece,
	iPiece
};

inline const std::vector<std::pair<char, WORD>> color = {
	{'T', FOREGROUND_BLUE | FOREGROUND_RED},
	{'J', FOREGROUND_BLUE},
	{'Z', FOREGROUND_RED},
	{'O', FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY},
	{'S', FOREGROUND_GREEN | FOREGROUND_INTENSITY},
	{'L', FOREGROUND_RED | FOREGROUND_GREEN},
	{'I', FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY}
};