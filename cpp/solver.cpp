
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <GameMatrix.h>

int main()
{
    const int N=4;
    uint game_data [N*N];
    uint m[N][N];
    char *game_string = getenv("QUERY_STRING");
    assert(game_string != NULL);

    int n_read = sscanf(game_string, "%d&%d&%d&%d&%d&%d&%d&%d&%d&%d&%d&%d&%d&%d&%d&%d", 
                        game_data, game_data+1, game_data+2, game_data+3,
                        game_data+4, game_data+5, game_data+6, game_data+7,
                        game_data+8, game_data+9, game_data+10, game_data+11,
                        game_data+12, game_data+13, game_data+14, game_data+15);

    assert(n_read == N*N);

    for (int i=0; i<N*N; i++) {
        m[i/N][i%N] = game_data[i];
    }

    GameMatrix gm(m);

    printf("Content-type: text/plain; charset=iso-8859-1\n\n");
    printf("%d\n", static_cast<int>(gm.get_best_move()));

    return 0;
}

