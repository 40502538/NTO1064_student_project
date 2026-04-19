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

            string resultLabel = parseExpressions();

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
        throw runtime_error("Unknown label: " + label);
    }
    string getRootLabel() const { return rootLabel; }


    vector<string> usedVars() const {
        set<string> vs;
        for (auto& t : tokens) if (t=="A"||t=="B"||t=="C") vs.insert(t);
        return vector<string>(vs.begin(), vs.end());
    }
    
    int operatorCount() const {
        int cnt = 0;
        for (auto& se : subExprs)
            if (se.op != "VAR") cnt++;
        return cnt;
    }

private:
    vector<string> tokens;
    int pos;
    string rootLabel;

vector<string> tokenise(const string& s){
    vector<string> result;
    istringstream  iss(s);
    string         word;
    while (iss >> word) {
      
        transform(word.begin(), word.end(), word.begin(), ::toupper);
        
        string prefix, core, suffix;
        size_t start = 0;
        while (start < word.size() && word[start] == '(') { prefix += '('; start++; }
        size_t end = word.size();
        while (end > start && word[end-1] == ')') { suffix += ')'; end--; }
        core = word.substr(start, end - start);
        for (char c : prefix)  result.push_back(string(1,c));
        if (!core.empty())     result.push_back(core);
        for (char c : suffix)  result.push_back(string(1,c));
    }
    return result;
}
    string parseExpr() {
        string left = parseTerm();

        while (pos < (int)tokens.size()) {
            string op = tokens[pos];
            if (op!="AND" && op!="OR" && op!="XOR" && op!="NAND" && op!="NOR")
                break;
            pos++;
            string right = parseTerm();

            string label = left + " " + op + " " + right;
            registerSubExpr(label, op, left, right);
            usedOps.insert(op);
            left = label;
        }
        return left;
    }
    string parseTerm() {
        if (pos >= (int)tokens.size())
            throw runtime_error("Unexpected end of expression");

        string tok = tokens[pos];

        if (tok == "NOT") {
            pos++;
            string operand = parseTerm();
            string label   = "NOT " + operand;
            registerSubExpr(label, "NOT", operand, "");
            usedOps.insert("NOT");
            return label;
        }
        if (tok == "A" || tok == "B" || tok == "C") {
            pos++;
            return tok; // variables are leaf nodes
        }
         throw runtime_error("Unknown token: " + tok);
    }
    void registerSubExpr(const string& label, const string& op,
                          const string& left,  const string& right) {
        for (auto& se : subExprs)
            if (se.label == label) return;
        subExprs.push_back({label, op, left, right});
    }
};

class TruthTable{
    public:
        void generate(BooleanExpression& expr){
            vars = expr.usedVars();
            cols = buildColumns(expr);
            rows = buildRows(expr);
            rootCol = expr.getRootLabel();
        }

        void print(ostream& out) const {
       
        out << "\n";
        printRow(out, buildHeaderCells());
        printSeparator(out);

        // --- data rows ---
        for (auto& row : rows)
            printRow(out, row);

        out << "\n";
    }
    const vector<string>&   getColumns() const { return cols; }
    const vector<vector<string>>&   getRows()    const { return rows; }
private:
    vector<string>         vars;
    vector<string>         cols;   
    vector<vector<string>> rows;
    string                 rootCol;
}




