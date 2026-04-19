// NTO1064 Student Project: Boolean Expression & Truth Table Simulator (C++ OOP)//

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

//*Section 1 - Base Class*//
class BooleanOperator {
    public:
        virtual bool evaluate(bool a, bool b = false)  const = 0; 
        virtual string getName()        const = 0;  
        virtual string getExplanation() const = 0;  
        

        virtual ~BooleanOperator() {}
};

//*Section 2 - Derived Operator Classes*//
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

//*Section 3 - Expression Parser and Evaluator*//
class BooleanExpression{
    public:
        string raw;                     // original expression

        struct SubExpr{
            string label;               // column header
            string op;                  // boolean operators
            string left;                // left operand label
            string right;               // right operand label
        };

        vector<SubExpr> subExprs;
        set<string> usedOps;


        void parse (const string& expression){
            raw = expression;
            subExprs.clear();
            usedOps.clear();
        

            tokens = tokenise(expression);
            pos = 0;

            string resultLabel = parseExpr();

            if (pos != (int)tokens.size())
                throw runtime_error("Unexpected token: " + tokens[pos]);

            rootLabel = resultLabel;
        }

        bool evaluate(const map<string,bool>& vars) const {
        return evalLabel(rootLabel, vars);
        }
    
        bool evalLabel(const string& label, const map<string,bool>& vars) const {
      
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

    //tokeniser
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
            return tok; 
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


//* Section 4 - Truth Table *//
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

    vector<string> buildColumns(BooleanExpression& expr) {
        vector<string> result;
        
        for (auto& v : vars) result.push_back(v);
        for (auto& se : expr.subExprs)
            if (se.label != expr.getRootLabel())
                result.push_back(se.label);
        result.push_back(expr.getRootLabel());
        return result;
    }

    vector<vector<string>> buildRows(BooleanExpression& expr) {
        int n       = (int)vars.size();
        int combos  = 1 << n;             
        vector<vector<string>> result;

        for (int mask = 0; mask < combos; mask++) {
            
            map<string,bool> assignment;
            for (int i = 0; i < n; i++) {
                
                bool val = (mask >> (n - 1 - i)) & 1;
                assignment[vars[i]] = val;
            }

            vector<string> row;
            for (auto& col : cols) {
                bool val = expr.evalLabel(col, assignment);
                row.push_back(val ? "1" : "0");
            }
            result.push_back(row);
        }
        return result;
    }


    vector<string> buildHeaderCells() const { return cols; }
  
    int colWidth(int i) const {
        int w = (int)cols[i].size();
        return max(w, 3);
    }

    void printRow(ostream& out, const vector<string>& cells) const {
        out << "|";
        for (int i = 0; i < (int)cols.size(); i++) {
            int   w    = colWidth(i);
            
            int   len  = (int)cells[i].size();
            int   pad  = w - len;
            int   left = pad / 2;
            int   right= pad - left;
            out << " " << string(left,' ') << cells[i] << string(right,' ') << " |";
        }
        out << "\n";
    }

    void printSeparator(ostream& out) const {
        out << "|";
        for (int i = 0; i < (int)cols.size(); i++) {
            int w = colWidth(i);
            out << "-" << string(w, '-') << "-|";
        }
        out << "\n";
    }
    
};

//* Section 5 - Operator Factory *//

unique_ptr<BooleanOperator> makeOperator(const string& name) {
    if (name == "AND")  return make_unique<AND_Operator>();
    if (name == "OR")   return make_unique<OR_Operator>();
    if (name == "NOT")  return make_unique<NOT_Operator>();
    if (name == "XOR")  return make_unique<XOR_Operator>();
    if (name == "NAND") return make_unique<NAND_Operator>();
    if (name == "NOR")  return make_unique<NOR_Operator>();
    throw runtime_error("Unknown operator: " + name);
}

//* Section 6 - File Save *//

void saveToFile(const string& filename,
                const BooleanExpression& expr,
                const TruthTable& table,
                const string& explanations)

{
    ofstream f(filename);
    if (!f) throw runtime_error("Cannot open file: " + filename);

    f << "=== BOOLEAN EXPRESSION SIMULATOR ===\n\n";
    f << "Expression : " << expr.raw << "\n\n";
    f << "Operators Detected:\n" << explanations << "\n";
    f << "Truth Table:\n";
    table.print(f);

    f.close();
    cout << "\nSaved to \"" << filename << "\" successfully!\n";
}

void loadFromFile(const string& filename) {
    ifstream f(filename);
    if (!f) throw runtime_error("Cannot open file: " + filename);

    cout << "\n--- Loaded from \"" << filename << "\" ---\n";
    string line;
    while (getline(f, line)) cout << line << "\n";
    cout << "--- End of file ---\n\n";
}

//* Section 7 - Main Loop *//

int main(){
    cout << "\n*** BOOLEAN TRUTH TABLE SIMULATOR ***\n\n";
    cout << "Commands:\n";
    cout << "  Type an expression to evaluate  (e.g.  (A AND B) OR (NOT C))\n";
    cout << "  Type  LOAD <filename>           to load a saved file\n";
    cout << "  Type  QUIT                      to exit\n\n";

    while (true) {
        cout << "Enter Boolean Expression: ";
        string line;
        if (!getline(cin, line)) break;

        // Trim whitespace
        while (!line.empty() && isspace(line.front())) line.erase(line.begin());
        while (!line.empty() && isspace(line.back()))  line.pop_back();

        if (line.empty()) continue;

        // Quit
        string upper = line;
        transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
        if (upper == "QUIT") break;

        // Load
        if (upper.substr(0, 5) == "LOAD ") {
            string fname = line.substr(5);
            while (!fname.empty() && isspace(fname.front())) fname.erase(fname.begin());
            try { loadFromFile(fname); }
            catch (exception& e) { cout << "Error: " << e.what() << "\n"; }
            continue;
    }

    BooleanExpression expr;
        try {
            expr.parse(line);
        } catch (exception& e) {
            cout << "Parse error: " << e.what() << "\n\n";
            continue;
        }

        if (expr.operatorCount() > 3) {
            cout << "Error: Expression has more than 3 operators. Please simplify.\n\n";
            continue;
        }

        cout << "\nOperators Detected and Explained:\n";
        string explanations;
        for (const string& opName : expr.usedOps) {
            try {
                auto op = makeOperator(opName);
                string line = "  - " + op->getExplanation() + "\n";
                cout << line;
                explanations += line;
            } catch (...) {}
        }

        // Truth Table
        cout << "\nGenerating Truth Table...\n";
        TruthTable table;
        table.generate(expr);
        table.print(cout);
        
        // Save
        cout << "Save to file? (Y/N): ";
        string answer;
        getline(cin, answer);
        transform(answer.begin(), answer.end(), answer.begin(), ::toupper);
        if (!answer.empty() && answer[0] == 'Y') {
            cout << "Enter filename: ";
            string fname;
            getline(cin, fname);
            while (!fname.empty() && isspace(fname.front())) fname.erase(fname.begin());
            if (!fname.empty()) {
                try { saveToFile(fname, expr, table, explanations); }
                catch (exception& e) { cout << "Error saving: " << e.what() << "\n"; }
            }
        }

        cout << "\n";
    }

    cout << "Goodbye!\n";
    return 0;

}



