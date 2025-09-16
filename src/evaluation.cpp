#include <array>
#include <ranges>

#include "bitboard.hpp"
#include "common.hpp"
#include "eval_constants.hpp"
#include "position.hpp"
#include "psqt_state.hpp"

#include "evaluation.hpp"

#include "eval_types.hpp"
#include "square.hpp"

namespace Clockwork {

// clang-format off
const PScore PAWN_MAT   = S(304, 516);
const PScore KNIGHT_MAT = S(907, 1016);
const PScore BISHOP_MAT = S(936, 1126);
const PScore ROOK_MAT   = S(1363, 1832);
const PScore QUEEN_MAT  = S(3085, 3257);
const PScore TEMPO_VAL  = S(26, 18);

const PScore BISHOP_PAIR_VAL  = S(64, 200);
const PScore DOUBLED_PAWN_VAL = S(-104, -121);

const std::array<PScore, 9> KNIGHT_MOBILITY = {
    S(-226, -491), S(-176, -100), S(-87, 18), S(-33, 36), S(12, 49), S(46, 57), S(85, 59), S(146, 8), S(176, -58),
};
const std::array<PScore, 14> BISHOP_MOBILITY = {                                                                                                                    S(-164, -260), S(-122, -195), S(-82, -111), S(-34, -91), S(2, -54), S(30, -22), S(30, 3), S(42, 32), S(62, 25), S(45, 28), S(98, 38), S(121, -31), S(252, -126), S(234, -40),
};
const std::array<PScore, 15> ROOK_MOBILITY = {                                                                                                                      S(-604, 73), S(-123, -77), S(-44, -35), S(-47, -45), S(-30, 6), S(-10, 17), S(5, 18), S(32, 28), S(55, 31), S(91, 33), S(143, 9), S(143, 35), S(189, 12), S(172, 13), S(294, -96),
};
const std::array<PScore, 28> QUEEN_MOBILITY = {                                                                                                                     S(-133, 0), S(-87, 36), S(-254, -829), S(-185, -104), S(-167, -224), S(-133, -159), S(-120, -124), S(-106, -65), S(-102, -8), S(-92, 17), S(-66, 14), S(-57, 48), S(-21, -3), S(-36, 66), S(-15, 47), S(-34, 75), S(-7, 75), S(19, 36), S(-2, 46), S(39, 30), S(144, -93), S(234, -197), S(29, -37), S(426, -278), S(423, -374), S(944, -718), S(547, -636), S(1304, -988),
};
const std::array<PScore, 9> KING_MOBILITY = {
    S(165, 623), S(120, 488), S(24, 479), S(13, 507), S(-7, 496), S(-51, 473), S(-47, 471), S(-43, 429), S(66, 310),
};

const std::array<PScore, 3> KNIGHT_KING_RING = {
    CS(0, 0), S(71, -35), S(135, -84),
};
const std::array<PScore, 3> BISHOP_KING_RING = {
    CS(0, 0), S(74, -14), S(151, -70),
};
const std::array<PScore, 5> ROOK_KING_RING = {
    CS(0, 0), S(82, -51), S(148, -87), S(223, -75), S(264, -89),
};
const std::array<PScore, 6> QUEEN_KING_RING = {
    CS(0, 0), S(2, 44), S(99, 20), S(297, -64), S(525, -80), S(754, -208),
};

const std::array<PScore, 48> PAWN_PSQT = {
    S(-15, 413),    S(-12, 442),    S(115, 400),    S(265, 261),    S(318, 167),    S(286, 310),    S(193, 356),    S(5, 430),      //
    S(58, 89),      S(7, 138),      S(199, 1),      S(134, 25),     S(142, 10),     S(64, 17),      S(64, 80),      S(-117, 124),   //
    S(-90, -80),    S(-18, -71),    S(-53, -107),   S(-38, -137),   S(-55, -130),   S(-72, -126),   S(-115, -57),   S(-168, -29),   //
    S(-66, -181),   S(-26, -130),   S(-61, -160),   S(-53, -185),   S(-71, -179),   S(-116, -169),  S(-120, -110),  S(-152, -127),  //
    S(-37, -212),   S(76, -202),    S(-33, -147),   S(-55, -167),   S(-92, -148),   S(-131, -162),  S(-126, -116),  S(-188, -124),  //
    S(-59, -233),   S(129, -205),   S(21, -166),    S(-56, -123),   S(-67, -196),   S(-122, -160),  S(-128, -120),  S(-154, -141),  //
};
const std::array<PScore, 64> KNIGHT_PSQT = {
    S(-295, -111),  S(-71, -64),    S(-464, 193),   S(169, 109),    S(0, -49),      S(-189, 11),    S(-302, 119),   S(-388, -43),   //
    S(328, -322),   S(215, -101),   S(178, -84),    S(153, -1),     S(212, 28),     S(141, -38),    S(104, -111),   S(75, -29),     //
    S(202, -38),    S(107, 271),    S(304, 32),     S(272, 31),     S(126, 91),     S(39, 162),     S(-55, 131),    S(-45, 151),    //
    S(158, -23),    S(34, 48),      S(106, 87),     S(-8, 177),     S(100, 130),    S(76, 104),     S(32, -6),      S(-95, 119),    //
    S(56, 40),      S(153, 29),     S(73, 77),      S(79, 75),      S(-1, 153),     S(12, 128),     S(-47, 46),     S(-33, 18),     //
    S(-80, 37),     S(-32, 3),      S(-50, 21),     S(32, 67),      S(18, 43),      S(-84, 47),     S(-92, 5),      S(-155, -24),   //
    S(29, 6),       S(8, 34),       S(-150, 39),    S(-34, 48),     S(-62, 41),     S(-27, -31),    S(-31, -142),   S(-4, -213),    //
    S(-126, 237),   S(-36, 12),     S(47, -138),    S(-23, -50),    S(-12, -15),    S(13, -93),     S(2, -101),     S(-296, -243),  //
};
const std::array<PScore, 64> BISHOP_PSQT = {
    S(-355, 185),   S(142, 79),     S(-389, 51),    S(-41, 42),     S(155, -29),    S(-46, 81),     S(-92, 149),    S(-315, 211),   //
    S(-90, -43),    S(-48, -13),    S(91, -31),     S(-136, 124),   S(-111, 125),   S(25, 33),      S(45, 2),       S(32, 13),      //
    S(-97, 90),     S(124, 43),     S(252, 10),     S(39, 29),      S(91, 17),      S(72, 20),      S(60, 34),      S(7, 0),        //
    S(-33, -40),    S(23, 15),      S(50, 55),      S(48, 79),      S(134, 57),     S(9, 80),       S(47, 64),      S(-59, 11),     //
    S(80, 21),      S(-25, 44),     S(35, 39),      S(68, 34),      S(82, 36),      S(15, 57),      S(-54, 66),     S(16, -16),     //
    S(84, -122),    S(69, 25),      S(58, -6),      S(49, 54),      S(26, 31),      S(16, 41),      S(47, 11),      S(-69, 82),     //
    S(-55, -36),    S(83, -2),      S(96, -77),     S(-3, 8),       S(-15, 8),      S(48, -6),      S(-29, 10),     S(9, -136),     //
    S(15, -103),    S(-71, -67),    S(-18, 42),     S(-27, 26),     S(-48, 53),     S(24, 19),      S(112, -138),   S(-206, 26),    //
};
const std::array<PScore, 64> ROOK_PSQT = {
    S(345, -45),    S(187, 16),     S(338, -14),    S(241, -47),    S(75, 29),      S(-25, 89),     S(254, 1),      S(142, 13),     //
    S(250, 22),     S(75, 142),     S(307, 41),     S(22, 150),     S(18, 173),     S(-6, 170),     S(-23, 136),    S(-50, 109),    //
    S(64, 80),      S(142, 59),     S(309, 18),     S(238, -15),    S(130, 31),     S(98, 69),      S(49, 100),     S(-7, 78),      //
    S(-95, 74),     S(12, 63),      S(39, 70),      S(97, 5),       S(124, 71),     S(22, 54),      S(-59, 104),    S(-8, 53),      //
    S(-131, 32),    S(-30, 30),     S(8, -18),      S(-41, 25),     S(-42, 9),      S(-73, 86),     S(-66, 5),      S(-191, 84),    //
    S(-158, 3),     S(-11, -64),    S(-56, -51),    S(-26, -43),    S(-87, 5),      S(-84, -38),    S(-72, -17),    S(-107, -16),   //
    S(-224, 14),    S(17, -85),     S(-61, -18),    S(-51, -25),    S(-55, -30),    S(-90, -23),    S(-106, -63),   S(-184, -35),   //
    S(-134, -27),   S(-113, -13),   S(-64, -44),    S(-31, -36),    S(-38, -22),    S(-66, -2),     S(-99, -7),     S(-103, -9),    //
};
const std::array<PScore, 64> QUEEN_PSQT = {
    S(174, 5),      S(239, -61),    S(6, 132),      S(-57, 153),    S(-129, 177),   S(-245, 249),   S(-80, 179),    S(-141, 24),    //
    S(93, 116),     S(58, 142),     S(176, 82),     S(-160, 298),   S(-139, 256),   S(-154, 317),   S(-70, 69),     S(-36, 76),     //
    S(-116, 305),   S(56, 263),     S(97, 194),     S(-1, 225),     S(-81, 223),    S(-9, 166),     S(-101, 216),   S(0, 15),       //
    S(45, 80),      S(-24, 224),    S(-36, 219),    S(-80, 279),    S(-58, 202),    S(-108, 229),   S(51, -44),     S(-20, -2),     //
    S(84, 10),      S(83, -31),     S(-5, 114),     S(-63, 148),    S(-27, 174),    S(-63, 135),    S(-40, 57),     S(0, -63),      //
    S(85, -116),    S(61, -42),     S(38, -32),     S(-10, 31),     S(-26, 45),     S(-53, 48),     S(-14, 42),     S(-109, 86),    //
    S(45, -167),    S(17, -316),    S(54, -201),    S(63, -121),    S(38, -194),    S(6, -78),      S(-91, 9),      S(50, -192),    //
    S(-189, -122),  S(-108, -419),  S(-210, -73),   S(53, -162),    S(33, -165),    S(74, -229),    S(31, -64),     S(-51, -78),    //
};
const std::array<PScore, 64> KING_PSQT = {
    S(500, -386),   S(282, -95),    S(-260, -86),   S(-281, 387),   S(0, 0),        S(0, 0),        S(0, 0),        S(0, 0),        //
    S(-8, -26),     S(-90, 95),     S(-251, 203),   S(-69, 132),    S(0, 0),        S(0, 0),        S(0, 0),        S(0, 0),        //
    S(-213, 114),   S(5, 180),      S(-147, 215),   S(28, 140),     S(0, 0),        S(0, 0),        S(0, 0),        S(0, 0),        //
    S(98, -82),     S(-42, 163),    S(-2, 123),     S(-13, 114),    S(0, 0),        S(0, 0),        S(0, 0),        S(0, 0),        //
    S(-480, 67),    S(-24, 66),     S(-153, 118),   S(-51, 104),    S(0, 0),        S(0, 0),        S(0, 0),        S(0, 0),        //
    S(-165, -31),   S(-22, 6),      S(-116, 63),    S(-115, 110),   S(0, 0),        S(0, 0),        S(0, 0),        S(0, 0),        //
    S(103, -168),   S(163, -70),    S(-31, 29),     S(-126, 96),    S(0, 0),        S(0, 0),        S(0, 0),        S(0, 0),        //
    S(60, -254),    S(136, -188),   S(-25, -121),   S(0, -176),     S(0, 0),        S(0, 0),        S(0, 0),        S(0, 0),        //
};
// clang-format on

std::array<Bitboard, 64> king_ring_table = []() {
    std::array<Bitboard, 64> king_ring_table{};
    for (u8 sq_idx = 0; sq_idx < 64; sq_idx++) {
        Bitboard sq_bb     = Bitboard::from_square(Square{sq_idx});
        Bitboard king_ring = sq_bb;
        king_ring |= sq_bb.shift(Direction::North);
        king_ring |= sq_bb.shift(Direction::South);
        king_ring |= sq_bb.shift(Direction::East);
        king_ring |= sq_bb.shift(Direction::West);
        king_ring |= sq_bb.shift(Direction::NorthEast);
        king_ring |= sq_bb.shift(Direction::SouthEast);
        king_ring |= sq_bb.shift(Direction::NorthWest);
        king_ring |= sq_bb.shift(Direction::SouthWest);
        king_ring_table[sq_idx] = king_ring;
    }
    return king_ring_table;
}();

template<Color color>
PScore evaluate_pawns(const Position& pos) {
    Bitboard pawns = pos.board().bitboard_for(color, PieceType::Pawn);
    PScore eval = PSCORE_ZERO;
    eval += DOUBLED_PAWN_VAL * (pawns & pawns.shift(Direction::North)).popcount();

    return eval;
}

template<Color color>
PScore evaluate_pieces(const Position& pos) {
    constexpr Color opp = ~color;
    PScore eval = PSCORE_ZERO;
    Bitboard bb = pos.bitboard_for(color, PieceType::Pawn) | pos.attacked_by(opp, PieceType::Pawn);
    Bitboard opp_king_ring = king_ring_table[pos.king_sq(opp).raw];
    for (PieceId id : pos.get_piece_mask(color, PieceType::Knight)) {
        eval += KNIGHT_MOBILITY[pos.mobility_of(color, id, ~bb)];
        eval += KNIGHT_KING_RING[pos.mobility_of(color, id, opp_king_ring)];
    }
    for (PieceId id : pos.get_piece_mask(color, PieceType::Bishop)) {
        eval += BISHOP_MOBILITY[pos.mobility_of(color, id, ~bb)];
        eval += BISHOP_KING_RING[pos.mobility_of(color, id, opp_king_ring)];
    }
    for (PieceId id : pos.get_piece_mask(color, PieceType::Rook)) {
        eval += ROOK_MOBILITY[pos.mobility_of(color, id, ~bb)];
        eval += ROOK_KING_RING[pos.mobility_of(color, id, opp_king_ring)];
    }
    for (PieceId id : pos.get_piece_mask(color, PieceType::Queen)) {
        eval += QUEEN_MOBILITY[pos.mobility_of(color, id, ~bb)];
        eval += QUEEN_KING_RING[pos.mobility_of(color, id, opp_king_ring)];
    }
    eval += KING_MOBILITY[pos.mobility_of(color, PieceId::king(), ~bb)];

    if (pos.piece_count(color, PieceType::Bishop) >= 2) {
        eval += BISHOP_PAIR_VAL;
    }

    return eval;
}

Score evaluate_white_pov(const Position& pos, const PsqtState& psqt_state) {
    const Color us    = pos.active_color();
    i32         phase = pos.piece_count(Color::White, PieceType::Knight)
              + pos.piece_count(Color::Black, PieceType::Knight)
              + pos.piece_count(Color::White, PieceType::Bishop)
              + pos.piece_count(Color::Black, PieceType::Bishop)
              + 2
                  * (pos.piece_count(Color::White, PieceType::Rook)
                     + pos.piece_count(Color::Black, PieceType::Rook))
              + 4
                  * (pos.piece_count(Color::White, PieceType::Queen)
                     + pos.piece_count(Color::Black, PieceType::Queen));

    phase = std::min<i32>(phase, 24);

    PScore mobility    = PSCORE_ZERO;
    PScore king_attack = PSCORE_ZERO;

    PScore eval = psqt_state.score();
    eval += evaluate_pieces<Color::White>(pos) - evaluate_pieces<Color::Black>(pos);
    eval += evaluate_pawns<Color::White>(pos) - evaluate_pawns<Color::Black>(pos);
    eval += (us == Color::White) ? TEMPO_VAL : -TEMPO_VAL;
    return eval->phase<24>(phase);
};

Score evaluate_stm_pov(const Position& pos, const PsqtState& psqt_state) {
    const Color us = pos.active_color();
    return (us == Color::White) ? evaluate_white_pov(pos, psqt_state)
                                : -evaluate_white_pov(pos, psqt_state);
}

}  // namespace Clockwork
