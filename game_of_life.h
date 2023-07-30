#ifndef GAME_OF_LIFE_H
#define GAME_OF_LIFE_H

#include <thread>
#include <exception>
#include <SFML/Graphics.hpp>

namespace game_of_life{

    // Declare standard colors to plot the cell status
    const sf::Color DEAD(0, 0, 0);
    const sf::Color ALIVE(255, 255, 255);

    // Predeclare Grid class and engine function to allow for friendship with Matrix class
    class Grid;
    template <typename mytype> class Matrix;
    void engine(Matrix<bool> start, int pixels, int delay);

    // Define a template Matrix class to handle memory access in a simple user friendly way
    template <typename mytype>
    class Matrix{
        private:
            bool init;
            int nrows, ncols;
            mytype* values;

            // Define a simple helper function to copy the data from a Matrix object to another
            void copy(const Matrix<mytype>& source){
                if(init==true) delete[] values;
                init=source.init; nrows=source.nrows; ncols=source.ncols;
                if(init==true){
                    values = new mytype [nrows*ncols];
                    for(int i=0; i<nrows*ncols; i++) values[i] = source.values[i];
                }
            }

        public:

            // Default constructor to build an unititialized matrix object
            Matrix() : init(false) {};

            // Constructor building an empty Matrix object 
            Matrix(int nrows_, int ncols_) : init(true), nrows(nrows_), ncols(ncols_) {
                if(nrows_<=0 || ncols_ <=0) throw std::runtime_error("Matrix dimensions must be a positive integers");
                values = new mytype [nrows*ncols];
            }

            // Constructor copying the data from a vector object
            Matrix(int nrows_, int ncols_, std::vector<mytype> values_) : Matrix(nrows_, ncols_) {
                for(int i=0; i<nrows*ncols; i++) values[i] = values_[i];
            }

            // Copy constructor
            Matrix(const Matrix<mytype>& source){
                copy(source);
            }
            
            // Copy assignment operator
            Matrix<mytype>& operator=(const Matrix<mytype>& source){
                copy(source);
                return *this;
            }
            
            // Destructor of the class
            ~Matrix(){
                if(init==true) delete[] values;
            }

            // Data access operator defined using the round brackets notation 
            mytype& operator()(int row, int col){
                if(init==false) throw std::runtime_error("Cannot operate on uninitialized object");
                if(row<0 || row>=nrows || col<0 || col>=ncols) throw std::runtime_error("Index out of Matrix bounds");
                return values[row + nrows*col];
            }

            friend class Grid;
            friend void engine(Matrix<bool> start, int pixels, int delay);
    };


    // Define Grid class to graphically represent the game of life
    class Grid : public sf::Drawable, public sf::Transformable{
        
        private:

            int nrows, ncols, width, height, pixelsize;
            Matrix<bool> status;
            sf::VertexArray pixelmap;

        public:

            // Constructor expecting a start matrix and a user defined pixelsize (size of the cells on the screen)
            Grid(Matrix<bool> start, int pixelsize_) : status(start), pixelsize(pixelsize_){
                nrows = status.nrows; ncols = status.ncols;
                width = ncols*pixelsize; height = nrows*pixelsize;
                pixelmap.setPrimitiveType(sf::Points);
                pixelmap.resize(width*height);
            }

            // Function to perform a single step in the evolution of the system
            void evolve(){

                // Define a neighbours matrix and compute the number of cells adjacent to the currently selected one
                Matrix<int> neighbours(nrows, ncols);
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
                
                // Update the status matrix using the number of neighbours computed in the previous step
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

            // Function to update the content of the graphical window
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
            // Define a draw function to make the Gird object drawable
            virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const{
                states.transform *= getTransform();
                states.texture = NULL;
                target.draw(pixelmap, states);
            }
    };

    // Define a simple engine function to animate the game
    void engine(Matrix<bool> start, int pixels, int delay){

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