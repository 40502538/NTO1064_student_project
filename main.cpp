#include <iostream>
#include <string>
using namespace std;

class BooleanOperator {
    public:
        virtual bool evaluate(bool a, bool b = false)  const = 0; 
        virtual string getName()        const = 0;  
        virtual string getExplanation() const = 0;  
        

        virtual ~BooleanOperator() {}
};

class AND_Operator : public BooleanOperator {
    public: 
        string getName()              const override {return "AND";}
        string getExplanation()       const override {
            return "AND : True only if both inputs are true.";
        }
    private:
        bool evaluate(bool a, bool b) const override { return a && b; }
};

class OR_Operator : public BooleanOperator {
    public: 
        string getName()              const override {return "OR";}
        string getExplanation()       const override {
            return "OR : True if at least one input is true.";
        }
    private:
        bool evaluate(bool a, bool b) const override { return a || b; }
};

class NOT_Operator : public BooleanOperator {
    public: 
        string getName()              const override {return "NOT";}
        string getExplanation()       const override {
            return "NOT : Inverts the input.";
        }
    private:
        bool evaluate(bool a, bool) const override { return !a; }
};

class NAND_Operator : public BooleanOperator {
    public: 
        string getName()              const override {return "NAND";}
        string getExplanation()       const override {
            return "NAND : Opposite of AND. False only if all inputs are true.";
        }
    private:
        bool evaluate(bool a, bool b) const override { return !(a && b); }
};

class NOR_Operator : public BooleanOperator {
    public: 
        string getName()              const override {return "NOR";}
        string getExplanation()       const override {
            return "NOR : Opposite of OR. True only if all inputs are false.";
        }
    private:
        bool evaluate(bool a, bool b) const override { return !(a || b); }
};

class XOR_Operator : public BooleanOperator {
    public: 
        string getName()              const override {return "XOR";}
        string getExplanation()       const override {
            return "XOR : True if two inputs are different.";
        }
    private:
        bool evaluate(bool a, bool b) const override { return a != b; }
};