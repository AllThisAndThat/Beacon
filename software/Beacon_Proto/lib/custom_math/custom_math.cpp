/*
Finds the number of characters that makes up an integer.
If the number is negative, adds one to account for "-" sign.
*/
int DigitsNumChars(const int x) {
    int y = x;
    if (x < 0) {
        y = -x;
    }

    y = (y < 10 ? 1 :   
        (y < 100 ? 2 :   
        (y < 1000 ? 3 :   
        (y < 10000 ? 4 :   
        (y < 100000 ? 5 :   
        (y < 1000000 ? 6 :   
        (y < 10000000 ? 7 :  
        (y < 100000000 ? 8 :  
        (y < 1000000000 ? 9 :  
        10)))))))));
        if (x < 0) {
            y++;
        }
        
    return y;
}