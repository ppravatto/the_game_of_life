#ifndef GAME_OF_LIFE_H
#define GAME_OF_LIFE_H

#include <exception>
#include <SFML/Graphics.hpp>

namespace game_of_life{

    // Declare standard colors to plot the cell status
    const sf::Color DEAD(0, 0, 0);
    const sf::Color ALIVE(255, 255, 255);


    // Define custom exception to avoid accessing uninitialized matrix objects
    struct InitializationException : public std::exception{
        const char* what() const throw(){
            return "Cannot operate on uninitialized object";
        }
    };

    // Define custom exception to avoid accessing non-initialized memory
    struct BoundException : public std::exception{
        const char* what() const throw(){
            return "Index of of matrix bounds";
        }
    };

    // Predeclare Grid class and engine function to allow for friendship with matrix class
    class Grid;
    template <typename mytype> class matrix;
    void engine(matrix<bool> start, int pixels, int delay);


    // Define a template matrix class to handle memory access in a simple user friendly way
    template <typename mytype>
    class matrix{
        private:
            bool init;
            int nrows, ncols;
            mytype* values;

            void copy(const matrix<mytype>& source){
                if(init==true) delete[] values;
                init=source.init; nrows=source.nrows; ncols=source.ncols;
                if(init==true){
                    values = new mytype [nrows*ncols];
                    for(int i=0; i<nrows*ncols; i++) values[i] = source.values[i];
                }
            }

        public:
            matrix() : init(false) {};

            matrix(int nrows_, int ncols_) : init(true), nrows(nrows_), ncols(ncols_) {
                values = new mytype [nrows*ncols];
            }

            matrix(int nrows_, int ncols_, std::vector<mytype> values_) : matrix(nrows_, ncols_) {
                for(int i=0; i<nrows*ncols; i++) values[i] = values_[i];
            }

            matrix(const matrix<mytype>& source){
                copy(source);
            }

            matrix<mytype>& operator=(const matrix<mytype>& source){
                copy(source);
                return *this;
            }

            ~matrix(){
                if(init==true) delete[] values;
            }

            mytype& operator()(int row, int col){
                if(init==false) throw InitializationException();
                if(row<0 || row>=nrows || col<0 || col>=ncols) throw BoundException();
                return values[row + nrows*col];
            }

            friend class Grid;
            friend void engine(matrix<bool> start, int pixels, int delay);
    };


    // Define Grid class to graphically represent the game of life
    class Grid : public sf::Drawable, public sf::Transformable{
        private:
            int nrows, ncols, width, height, pixelsize;
            matrix<bool> status;
            sf::VertexArray pixelmap;

        public:
            Grid(matrix<bool> start, int pixelsize_) : status(start), pixelsize(pixelsize_){
                nrows = status.nrows; ncols = status.ncols;
                width = ncols*pixelsize, height = nrows*pixelsize;
                pixelmap.setPrimitiveType(sf::Points);
                pixelmap.resize(width*height);
            }

            void evolve(){
                matrix<int> neighbours(nrows, ncols);
                for(int row=0; row<nrows; row++){
                    for(int col=0; col<ncols; col++){
                        int sum = 0;
                        for(int i=-1; i<=1; i++){
                            for(int j=-1; j<=1; j++){
                                int r=row+i, c=col+j;
                                if(r<0 || r>=nrows || c<0 || c>=ncols) continue; 
                                if(i!=0 || j!=0) sum += (status(r, c)==true)? 1 : 0;
                            }   
                        }
                        neighbours(row, col) = sum;
                    }
                }            

                for(int row=0; row<nrows; row++){
                    for(int col=0; col<ncols; col++){
                        if(status(row, col) == true){
                            if(neighbours(row, col)<2 || neighbours(row, col)>3) status(row, col) = false;
                        }
                        else{
                            if(neighbours(row, col)==3) status(row, col) = true;
                        }
                    }
                }
            }

            void update(){

                for(int row=0; row<nrows; row++){
                    for(int col=0; col<ncols; col++){
                        sf::Color color = (status(row, col)==true)? ALIVE : DEAD;
                        for(int r=0; r<pixelsize; r++){
                            for(int c=0; c<pixelsize; c++){
                                int x = col*pixelsize + c;
                                int y = row*pixelsize + r;
                                pixelmap[y+width*x].position = sf::Vector2f(x, y);
                                pixelmap[y+width*x].color = color;
                            }   
                        }
                    }
                }
            }

        private:
            virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const{
                states.transform *= getTransform();
                states.texture = NULL;
                target.draw(pixelmap, states);
            }
    };

    // Define a simple engine function to animate the game
    void engine(matrix<bool> start, int pixels, int delay){

        const int WIDTH = start.ncols*pixels;
        const int HEIGHT = start.nrows*pixels;

        Grid game_grid(start, pixels);

        sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "The game of life", sf::Style::Titlebar | sf::Style::Close);

        bool first = false;
        while(window.isOpen()){

            sf::Event event;
            while(window.pollEvent(event)){

                if(event.type==sf::Event::Closed){
                    window.close();
                }

            }

            window.clear(sf::Color::Black);

            if(first != false) game_grid.evolve();

            game_grid.update();
            window.draw(game_grid);

            window.display();

            if(first == false) first = true;

            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        }
    }

}

#endif