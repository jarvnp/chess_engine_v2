// https://www.chessprogramming.org/Zobrist_Hashing
#ifndef HASHER_H
#define HASHER_H

#include "boardpoint.h"
#include "move.h"
#include <unordered_map>

const int CASTLING_HASHES_N = 16;

class Hasher
{
public:
    Hasher();
    
    void makeAMove(moveBackupData& move);  // updates hashChange in "move"
    void reverseAMove(uint64_t hashChange);

    uint64_t hash;

private:
    uint64_t pieceHashes[18][64];
    uint64_t castlingHashes[CASTLING_HASHES_N];
    uint64_t blackMoveHash;
};

#endif // HASHER_H
