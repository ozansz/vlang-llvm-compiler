var i: int, ozan: real, hello: int[3];

int func WhatIsThisDoing(self: int, this: real)
    ozan := 51.678e-7;

    hello[0] := 6 + 2;
    hello[1] := hello[0] + 2;
    hello[2] := hello[1] - 2;

    var k: int;

    for k := 0 to 2 by 1
        print "hello[", k, "] = ", hello[k], "\n";
    endfor;

    hello[2] := hello[2] + hello[0];
    return hello[2];
endfunc

int func main()
    return WhatIsThisDoing(1, 4.7);
endfunc