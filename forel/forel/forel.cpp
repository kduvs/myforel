#include <iostream>
#include <vector>
#include <cmath>
#include <map>
#include <time.h>
#include <omp.h>
using namespace std;

double dist(vector<double> a, vector<double> b) {
    double d = 0;
#pragma omp parallel for
    for (int i = 0; i < a.size(); i++) {
        d += (a[i] - b[i]) * (a[i] - b[i]);
    }
    return sqrt(d);
}

vector<double> init_center(vector< vector<double> > data) {
    return data[rand() % data.size()];
}

vector< vector <double> > clustering(vector< vector<double> > data, vector<double> center, double width) {
    double range;
    vector< vector <double> > cluster;
#pragma omp parallel for
    for (int i = 0; i < data.size(); i++) {
        range = dist(center, data[i]);
        if (range <= width) {
            cluster.push_back(data[i]);
        }
    }
    //cout<<"clustering"<<endl;
    return cluster;
}

vector<double> avg(vector< vector <double> > cluster) {
    int i, j;
    vector<double> new_center(cluster[0].size());
    for (i = 0; i < cluster.size(); i++) {
        for (j = 0; j < cluster[0].size(); j++) {
            new_center[j] += cluster[i][j];
        }
    }
#pragma omp parallel for
    for (j = 0; j < cluster[0].size(); j++) {
        new_center[j] /= cluster.size();
    }
    //cout<<"avg"<<endl;
    return new_center;
}

void delete_points(vector< vector< double > >& data, vector< vector< double > > cluster) {
    for (int i = 0; i < cluster.size(); i++) {
        for (int j = 0; j < data.size(); j++) {
            if (cluster[i] == data[j]) {
                data.erase(data.begin() + j);
            }
        }
    }
}

void print_v1(vector<double> v) {
    int i, j;
    for (i = 0; i < v.size(); i++) {
        cout << v[i] << ' ';
    }
    cout << endl;
}

void print_v2(vector< vector<double> > data) {
    int i, j;
    for (i = 0; i < data.size(); i++) {
        for (j = 0; j < data[0].size(); j++) {
            cout << data[i][j] << ' ';
        }
        cout << endl;
    }
}

void print_m(map< int, vector< vector< double > > > m) {
    for (auto it = m.begin(); it != m.end(); it++)
    {
        cout << it->first << endl;
        print_v2(it->second);
    }
    cout << "size : " << m.size() << endl;
}

vector< vector<double> > init_data(int n, int m) {
    vector< vector<double> > data(n, vector<double>(m));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            data[i][j] = rand() % 10;
        }
    }
    return data;
}

map< int, vector< vector<double> > > forel(int n, int m, double width) {
    map< int, vector< vector<double> > > result;
    vector< vector< double > > data = init_data(n, m);
    for (int i = 0; !data.empty(); i++) {
        vector< double > old_center = init_center(data);
        vector< vector< double > > cluster = clustering(data, old_center, width);
        vector< double > new_center = avg(cluster);
        while (new_center != old_center) {
            old_center = new_center;
            cluster = clustering(data, old_center, width);
            new_center = avg(cluster);
        }
        result[i] = cluster;
        delete_points(data, cluster);
    }
    return result;
}

int main()
{
    int i, j, n, m, width;
    double c;

    clock_t start_time, finish_time;
    cout << "Введите ширину окна в процентах (0-100) от максимального расстояния между двумя точками : ";
    cin >> width;
    for (j = 1; j <= 4; j++) {
        omp_set_num_threads(j);
        for (i = 250; i <= 1000; i += 250) {
            cout << i << endl;
            start_time = clock();
            c = 9.0 * pow(i, 0.5) * width / 100;
            map< int, vector< vector<double> > > f = forel(i, i, c);
            //print_m(f);
            //cout<<"c : "<<c<<", "<<9.0 * pow(i, 0.5)<<endl;
            finish_time = clock();
            cout << double(finish_time - start_time) / CLOCKS_PER_SEC << " seconds" << endl;
        }
    }
    cout << "help me, i am dead inside";
    return 0;
}
