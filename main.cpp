#include <iostream>
using namespace std;

class BooleanOperator {
    public:
        virtual bool respond(bool a, bool b) { return 0; }
        virtual bool respond(int a) { return 0; }
};

class AND_Operator : public BooleanOperator {
    private:
        bool respond(bool a, bool b) override { return a && b; }
};

class OR_Operator : public BooleanOperator {
    private:
        bool respond(bool a, bool b) override { return a || b; }
};

class NOT_Operator : public BooleanOperator {
    private:
        bool respond(int a) override { return !a; }
};

class NAND_Operator : public BooleanOperator {
    private:
        bool respond(bool a, bool b) override { return !(a && b); }
};

class NOR_Operator : public BooleanOperator {
    private:
        bool respond(bool a, bool b) override { return !(a || b); }
};

class XOR_Operator : public BooleanOperator {
    private:
        bool respond(bool a, bool b) override { return a != b; }
};