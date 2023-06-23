void replace(const string & c_filename, int line_number, const string & synonymous_code) {
    ifstream fin(c_filename);
    if (!fin) return ERROR_INF;
    vector < string > lines;
    string line;
    while (getline(fin, line)) lines.push_back(line);
    if (line_number < 1 | | line_number > lines.size()) return ERROR_INF;
    lines[line_number - 1] = synonymous_code + "\n";
    ofstream fout(c_filename);
    if (!fout) return ERROR_INF;
    for (const auto & line: lines) fout << line; 
}
