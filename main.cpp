#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <stdexcept>
#include <memory>    

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


class BooleanExpression{
    public:
        string raw;

        struct SubExpression{
            string label;
            string op;
            string left;
            string right;
        };

        vector<SubExpression> subExprs;
        set<string> usedOps;


        void parse (const string& expression){
            raw = expression;
            subExprs.clear();
            usedOps.clear();
        

            tokens = tokenise(expression);
            pos = 0;

            string resultLabel = parseExpression();

            if (pos != (int)tokens.size())
                throw runtime_error("Unexpected token: " + tokens[pos]);

            rootLabel = resultLabel;


        }

        bool evaluate(const map<string,bool>& vars) const {
        return evalLabel(rootLabel, vars);
    }
        bool evalLabel(const string& label, const map<string,bool>& vars) const {
        // Is it a variable?
        if (label == "A" || label == "B" || label == "C") {
            auto it = vars.find(label);
            return (it != vars.end()) ? it->second : false;
        }
    
        for (const auto& se : subExprs) {
            if (se.label == label) {
                if (se.op == "VAR") {
                    return vars.at(se.left);
                }
                bool lv = evalLabel(se.left, vars);
                if (se.op == "NOT") return !lv;
                bool rv = evalLabel(se.right, vars);
                if (se.op == "AND")  return lv && rv;
                if (se.op == "OR")   return lv || rv;
                if (se.op == "XOR")  return lv != rv;
                if (se.op == "NAND") return !(lv && rv);
                if (se.op == "NOR")  return !(lv || rv);
            }
        }

};


