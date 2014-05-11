
#include <gtest/gtest.h>
#include <GameMatrix.h>
#include <Simulator.h>

TEST(GameMatrixTest, GetPosTest)
{
    uint matrix[4][4]   = {{0,0,0,0},{0,2,0,0},{0,0,2,0},{0,0,0,0}};
    GameMatrix gm(matrix);
    EXPECT_TRUE(*gm._get_pos(2, 3, UP) == position(3, 2));
    EXPECT_TRUE(*gm._get_pos(2, 3, DOWN) == position(0, 2));
    EXPECT_TRUE(*gm._get_pos(2, 3, RIGHT) == position(2, 0));
    EXPECT_TRUE(*gm._get_pos(2, 3, LEFT) == position(2, 3));
}

TEST(GameMatrixTest, SimulateMoveSimple)
{
    uint ref[4][4]   = {{0,0,0,0},{0,2,0,0},{0,0,2,0},{0,0,0,0}};
    uint exp[4][4][4]    = {{{0,2,2,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}},
                            {{0,0,0,0},{0,0,0,2},{0,0,0,2},{0,0,0,0}},
                            {{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,2,2,0}},
                            {{0,0,0,0},{2,0,0,0},{2,0,0,0},{0,0,0,0}}};
    GameMatrix g_ref(ref);
    for (uint m=0; m<4; ++m)
    {
        GameMatrix g_exp(exp[m]);
        GameMatrix g_res = g_ref.simulate_move((move)m);
        EXPECT_TRUE(g_exp == g_res);
    }
}

TEST(GameMatrixTest, SimulateMoveMoreComplex)
{
    uint ref[4][4]   = {{2,2,2,2},{2,2,2,2},{4,4,4,4},{8,8,8,8}};
    uint exp[4][4][4]    = {{{4,4,4,4},{4,4,4,4},{8,8,8,8},{0,0,0,0}},
                            {{0,0,4,4},{0,0,4,4},{0,0,8,8},{0,0,16,16}},
                            {{0,0,0,0},{4,4,4,4},{4,4,4,4},{8,8,8,8}},
                            {{4,4,0,0},{4,4,0,0},{8,8,0,0},{16,16,0,0}}};
    GameMatrix g_ref(ref);
    for (uint m=0; m<4; ++m)
    {
        GameMatrix g_exp(exp[m]);
        GameMatrix g_res = g_ref.simulate_move((move)m);
        EXPECT_TRUE(g_exp == g_res);
    }
}

TEST(GameMatrixTest, SimulateMoveUniformMatrix)
{
    uint ref[4][4]   = {{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2}};
    uint exp[4][4][4]    = {{{4,4,4,4},{4,4,4,4},{0,0,0,0},{0,0,0,0}},
                            {{0,0,8,8},{0,0,8,8},{0,0,0,0},{0,0,0,0}},
                            {{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,16,16}},
                            {{0,0,0,0},{0,0,0,0},{0,0,0,0},{32,0,0,0}}};
    GameMatrix curr_mat(ref);
    for (uint m=0; m<4; ++m)
    {
        GameMatrix g_exp(exp[m]);
        GameMatrix g_new = curr_mat.simulate_move((move)m);
        EXPECT_TRUE(g_new == g_exp);
        curr_mat = g_new;
    }
}

TEST(GameMatrixTest, GetBestMoveSimple)
{
    uint ref[4][4]   = {{0,0,0,0},{0,0,2,0},{0,0,2,0},{0,0,0,0}};
    GameMatrix g_ref(ref);
    move best_move = g_ref.get_best_move();
    EXPECT_TRUE(best_move == UP || best_move == DOWN);
}

TEST(GameMatrixTest, ValidMove)
{
    uint matrix1[4][4]   = {{0,0,0,0},{0,0,2,0},{0,0,2,0},{0,0,2,0}};
    uint matrix2[4][4]   = {{0,0,0,0},{0,0,2,0},{0,0,4,0},{0,0,2,0}};

    GameMatrix gm1(matrix1);
    GameMatrix gm2(matrix2);

    EXPECT_TRUE(gm1.do_move(DOWN));
    EXPECT_FALSE(gm2.do_move(DOWN));
}

TEST(GameMatrixTest, CanMove)
{
    uint ref[4][4] = {{2,16,4,8},{256,64,32,4},{32,16,128,2},{2,4,2,8}};
    GameMatrix g_ref(ref);
    EXPECT_FALSE(g_ref.can_move());
}

TEST(SimulatorTest, SimpleGame)
{
    Simulator s;
    s.play();
}

TEST(SimulatorTest, MultipleGames)
{
    Simulator s;
    double average = 0;
    const uint max_games = 10;
    for (int i=0; i<max_games; ++i)
    {
        average += s.play();
    }
    average /= max_games;
    EXPECT_TRUE(average >= 550);
    std::cout << average << std::endl;
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
