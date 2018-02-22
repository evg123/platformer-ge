//
//  hopman.h
//  Main class for the Hopman platformer
//
//  Created by Vande Griek, Eric on 2/22/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#ifndef hopman_h
#define hopman_h

constexpr int ERROR = 1;
constexpr int OK = 0;

class Hopman {
private:
public:
    Hopman();
    ~Hopman();
    int play();
};

#endif /* hopman_h */
