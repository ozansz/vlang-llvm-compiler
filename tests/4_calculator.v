%%%%%%%%%%%%%%%%%%%%%%%%
% File: 4_calculator.v %
% Lang: VLang          %
% Author: Ozan Sazak   %
%%%%%%%%%%%%%%%%%%%%%%%%

var num1: int, num2: int, choice: int;

int func printOptions()
    print "Options: (1) Addition\n";
    print "         (2) Subtraction\n";
    print "         (3) Multiplication\n";
    print "         (4) Division\n";
    print "         (5) Power\n";
    print "         (6) Factorial\n";
    print "         (-1) to exit\n";

    return 0;
endfunc

int func factorial(number: int)
    if number <= 1 then
        return 1;
    endif;

    return (number * factorial(number - 1));
endfunc

int func power(base: int, pow: int)
    var p: int, i: int;

    p := 1;

    for i := 1 to pow by 1
        p := base * p;
    endfor;

    return p;
endfunc

int func main()
    var dummy: int;

    choice := 0;

    print "~~ Basic calculator ~~\n";
    
    while choice <> -1 do
        dummy := printOptions();

        print "choice > ";
        read choice;
        
        print "num1 > ";
        read num1;

        print "num2 > ";
        read num2;

        if choice = 1 then
            print num1, " + ", num2, " = ", num1 + num2, "\n";
            
        else if choice = 2 then
            print num1, " - ", num2, " = ", num1 - num2, "\n";

            else if choice = 3 then
                print num1, " * ", num2, " = ", num1 * num2, "\n";

                else if choice = 4 then
                    print num1, " / ", num2, " = ", num1 / num2, "\n";

                    else if choice = 5 then
                        print num1, " ^ ", num2, " = ", power(num1, num2), "\n";

                        else if choice = 6 then
                            print num1, " ! = ", factorial(num1), "\n";

                            else
                                print "Wrong or corrupted input: ", choice, "\n";
                            endif;          
                        endif;
                    endif;
                endif;
            endif;
        endif;
    endwhile;

    return 0;
endfunc
