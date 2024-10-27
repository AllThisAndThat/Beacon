#pragma once

#include <stdio.h>



/*
PinManager can keep track of all the objects it has
*/
class PinManager 
{
    public:
        static PinManager* Instance();

        //can write an enable for each type individually or find clever way to use one function
        void enableDigitalOutputObject(DigitalOutputObject* object);

    protected:
        PinManager();

    private:
        static PinManager *mInstance;
};
