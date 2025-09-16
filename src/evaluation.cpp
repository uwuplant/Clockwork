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
const PScore PAWN_MAT   = S(297, 520);
const PScore KNIGHT_MAT = S(873, 1040);
const PScore BISHOP_MAT = S(896, 1153);
const PScore ROOK_MAT   = S(1305, 1860);
const PScore QUEEN_MAT  = S(3078, 3193);
const PScore TEMPO_VAL  = S(34, 9);

const PScore BISHOP_PAIR_VAL  = S(68, 206);
const PScore DOUBLED_PAWN_VAL = S(-109, -114);

const std::array<PScore, 9> KNIGHT_MOBILITY = {
    S(-209, -305), S(-144, -154), S(-102, 32), S(-30, 20), S(3, 55), S(42, 66), S(84, 56), S(100, 58), S(136, -21),
};
const std::array<PScore, 14> BISHOP_MOBILITY = {
    S(-191, -322), S(-116, -238), S(-71, -137), S(-54, -64), S(-13, -41), S(17, -8), S(44, -7), S(48, 1), S(39, 47), S(28, 21), S(128, -3), S(83, -3), S(239, -111), S(204, -26),
};
const std::array<PScore, 15> ROOK_MOBILITY = {
    S(-654, -450), S(-61, -141), S(-54, -18), S(-69, -8), S(-25, 2), S(-26, 26), S(-14, 39), S(31, 21), S(60, 12), S(88, 25), S(107, 40), S(118, 47), S(200, -1), S(173, 18), S(214, -66),
};
const std::array<PScore, 28> QUEEN_MOBILITY = {
    S(-133, 0), S(-122, -68), S(-268, -412), S(-91, -284), S(-149, -205), S(-126, -224), S(-122, -99), S(-120, -34), S(-91, -44), S(-79, -29), S(-92, 68), S(-55, 44), S(-45, 63), S(-24, 49), S(-20, 60), S(-39, 65), S(-4, 74), S(12, 45), S(-48, 80), S(127, -33), S(158, -76), S(184, -139), S(56, -91), S(444, -420), S(455, -508), S(985, -808), S(480, -772), S(1059, -1337),
};
const std::array<PScore, 9> KING_MOBILITY = {
    S(238, 552), S(88, 618), S(4, 517), S(26, 500), S(7, 470), S(-41, 456), S(-56, 467), S(-36, 416), S(61, 297),
};

const std::array<PScore, 3> KNIGHT_KING_RING = {
    CS(0, 0), S(65, -32), S(150, -94),
};
const std::array<PScore, 3> BISHOP_KING_RING = {
    CS(0, 0), S(84, -37), S(172, -77),
};
const std::array<PScore, 5> ROOK_KING_RING = {
    CS(0, 0), S(74, -49), S(160, -82), S(212, -73), S(253, -136),
};
const std::array<PScore, 6> QUEEN_KING_RING = {
    CS(0, 0), S(9, 24), S(118, 3), S(243, -14), S(569, -147), S(883, -299),
};

const std::array<PScore, 48> PAWN_PSQT = {
    S(-118, 449),   S(-265, 592),   S(162, 447),    S(340, 125),    S(399, 143),    S(243, 257),    S(264, 343),    S(120, 351),    //
    S(35, 73),      S(65, 123),     S(211, -11),    S(182, -10),    S(52, 64),      S(78, 28),      S(18, 98),      S(-110, 109),   //
    S(-108, -35),   S(-9, -56),     S(-17, -131),   S(-58, -118),   S(-56, -121),   S(-90, -105),   S(-128, -48),   S(-147, -30),   //
    S(-67, -175),   S(-24, -125),   S(-30, -171),   S(-54, -185),   S(-77, -191),   S(-115, -179),  S(-137, -92),   S(-152, -122),  //
    S(-63, -187),   S(51, -194),    S(-11, -159),   S(-48, -175),   S(-83, -163),   S(-134, -152),  S(-128, -111),  S(-188, -134),  //
    S(-62, -237),   S(127, -218),   S(20, -167),    S(-84, -102),   S(-80, -189),   S(-134, -144),  S(-135, -109),  S(-164, -134),  //
};
const std::array<PScore, 64> KNIGHT_PSQT = {
    S(-188, -190),  S(-182, 289),   S(-490, 124),   S(243, 28),     S(-218, 187),   S(-206, 112),   S(-21, 134),    S(-395, -54),   //
    S(230, -169),   S(383, -77),    S(152, 19),     S(159, 96),     S(176, 51),     S(12, 94),      S(-39, 17),     S(24, 62),      //
    S(24, 76),      S(166, 82),     S(93, 131),     S(184, 128),    S(115, 95),     S(54, 107),     S(36, -11),     S(93, 39),      //
    S(98, -5),      S(18, 55),      S(83, 120),     S(-2, 131),     S(72, 105),     S(73, 73),      S(23, -15),     S(-29, 113),    //
    S(89, -23),     S(63, 55),      S(82, 34),      S(97, 41),      S(5, 105),      S(47, 56),      S(1, 27),       S(-16, 18),     //
    S(-76, -54),    S(-29, 10),     S(-44, 14),     S(61, 38),      S(34, 48),      S(-81, 36),     S(-124, 39),    S(-154, 28),    //
    S(100, -128),   S(-44, 79),     S(-176, 80),    S(-46, 46),     S(-62, 41),     S(8, -86),      S(-95, -90),    S(-55, -138),   //
    S(-156, 40),    S(-66, -23),    S(44, -172),    S(-28, -115),   S(-33, 7),      S(15, -209),    S(-47, 25),     S(-182, -144),  //
};
const std::array<PScore, 64> BISHOP_PSQT = {
    S(-401, 106),   S(333, 43),     S(-386, 44),    S(-362, 152),   S(-262, 158),   S(-335, 91),    S(-251, 127),   S(-194, 40),    //
    S(-82, -1),     S(-170, 99),    S(79, -23),     S(-73, 157),    S(-124, 29),    S(20, 41),      S(11, 26),      S(78, -20),     //
    S(-56, 128),    S(113, -22),    S(53, 132),     S(29, -2),      S(50, 45),      S(109, 45),     S(22, 11),      S(18, 18),      //
    S(35, -102),    S(29, 56),      S(33, 64),      S(87, 57),      S(57, 109),     S(-15, 83),     S(68, 5),       S(-18, -26),    //
    S(108, -86),    S(32, -62),     S(21, 42),      S(82, 35),      S(75, 50),      S(-10, 92),     S(-76, 82),     S(25, -51),     //
    S(66, -94),     S(85, -9),      S(92, -34),     S(47, 44),      S(25, 64),      S(-21, 86),     S(43, -2),      S(-85, 89),     //
    S(-13, -24),    S(96, -20),     S(55, -33),     S(-2, -15),     S(-52, 16),     S(39, 0),       S(-35, -6),     S(42, -49),     //
    S(74, -61),     S(-137, 22),    S(-1, 31),      S(-51, 8),      S(-43, 16),     S(14, 17),      S(134, -187),   S(-264, 42),    //
};
const std::array<PScore, 64> ROOK_PSQT = {
    S(262, 6),      S(106, 119),    S(236, 8),      S(105, 0),      S(79, 20),      S(10, 70),      S(147, 0),      S(83, 81),      //
    S(247, 0),      S(66, 123),     S(177, 103),    S(90, 94),      S(55, 155),     S(0, 106),      S(34, 82),      S(-4, 114),     //
    S(73, 77),      S(314, -2),     S(288, 13),     S(219, 5),      S(83, 66),      S(123, 73),     S(-17, 133),    S(-110, 167),   //
    S(-104, 79),    S(62, 2),       S(69, 36),      S(81, 24),      S(92, 55),      S(85, 36),      S(-88, 121),    S(-58, 98),     //
    S(-130, -38),   S(-60, 70),     S(-7, 26),      S(-94, 49),     S(-58, 38),     S(-99, 103),    S(-91, 19),     S(-186, 76),    //
    S(-150, -31),   S(-2, -74),     S(-1, -123),    S(-40, -63),    S(-81, 19),     S(-64, -21),    S(-107, 1),     S(-88, -51),    //
    S(-267, 54),    S(-9, -121),    S(-64, -52),    S(-46, -41),    S(-56, -8),     S(-87, -29),    S(-119, -17),   S(-218, -18),   //
    S(-169, -14),   S(-106, -9),    S(-68, -52),    S(-31, -51),    S(-57, -17),    S(-61, -24),    S(-84, -27),    S(-96, -42),    //
};
const std::array<PScore, 64> QUEEN_PSQT = {
    S(218, -46),    S(204, -8),     S(7, 158),      S(-119, 229),   S(-127, 124),   S(-101, 158),   S(37, 91),      S(-138, 68),    //
    S(146, 119),    S(-125, 359),   S(-13, 252),    S(-184, 295),   S(-154, 316),   S(-165, 318),   S(-106, 165),   S(0, 34),       //
    S(-131, 335),   S(113, 263),    S(170, 169),    S(8, 189),      S(-30, 187),    S(45, 119),     S(-33, 156),    S(7, -45),      //
    S(30, 128),     S(0, 177),      S(-20, 237),    S(-109, 341),   S(-46, 250),    S(-51, 192),    S(36, 21),      S(-116, 141),   //
    S(99, -16),     S(151, -163),   S(-12, 129),    S(-16, 149),    S(-42, 190),    S(26, 4),       S(-67, 45),     S(-28, 0),      //
    S(65, -116),    S(17, 37),      S(16, 37),      S(-5, 12),      S(-34, 51),     S(-46, 65),     S(-1, 9),       S(-87, 159),    //
    S(2, -219),     S(47, -367),    S(57, -165),    S(41, -120),    S(21, -152),    S(41, -167),    S(-137, 64),    S(-3, -73),     //
    S(-185, -37),   S(82, -578),    S(-176, -82),   S(101, -263),   S(37, -181),    S(66, -243),    S(16, -181),    S(-42, -122),   //
};
const std::array<PScore, 64> KING_PSQT = {
    S(402, -433),   S(256, -252),   S(-342, 21),    S(-228, 63),    S(0, 0),        S(0, 0),        S(0, 0),        S(0, 0),        //
    S(157, -217),   S(152, -23),    S(-320, 224),   S(187, 27),     S(0, 0),        S(0, 0),        S(0, 0),        S(0, 0),        //
    S(-27, -3),     S(-8, 243),     S(71, 131),     S(34, 171),     S(0, 0),        S(0, 0),        S(0, 0),        S(0, 0),        //
    S(-221, 76),    S(-82, 144),    S(-8, 112),     S(-59, 54),     S(0, 0),        S(0, 0),        S(0, 0),        S(0, 0),        //
    S(-314, 9),     S(-127, 117),   S(-150, 138),   S(-85, 101),    S(0, 0),        S(0, 0),        S(0, 0),        S(0, 0),        //
    S(-176, -62),   S(-34, 23),     S(-61, 33),     S(-117, 100),   S(0, 0),        S(0, 0),        S(0, 0),        S(0, 0),        //
    S(106, -162),   S(166, -51),    S(-26, 47),     S(-118, 97),    S(0, 0),        S(0, 0),        S(0, 0),        S(0, 0),        //
    S(54, -272),    S(127, -156),   S(-25, -115),   S(19, -174),    S(0, 0),        S(0, 0),        S(0, 0),        S(0, 0),        //
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
