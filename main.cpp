#include <iostream>
#include <chrono>
#include <thread>
#include <SFML/Graphics.hpp>

#include "game_of_life.h"

int main(){

    const int NCOLS=200, NROWS=200, PIXELS=5, MILLISECONDS=10;

    game_of_life::Matrix<bool> start(NROWS, NCOLS);
    for(int r=0; r<NROWS; r++){
        for(int c=0; c<NCOLS; c++){
            int i = r + NROWS*c;
            start(r, c) = (i%3==0 || i%5==0 || i%7==0 || i%9==0 || i%13==0)? false : true;
        }
    }

    game_of_life::engine(start, PIXELS, MILLISECONDS);

    return 0;
}