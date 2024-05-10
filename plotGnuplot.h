#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>

using namespace std;
typedef vector<double> Vector;

class GNUPLOT
{
private:
    Vector beat;    // results for beat group
    Vector meet;    // results for meet group
    Vector miss;    // results for miss group
    
public:
    // Default constructor
    GNUPLOT() {}

    // Parameterized constructor
    GNUPLOT(const Vector& beat_, const Vector& meet_, const Vector& miss_)
        : beat(beat_), meet(meet_), miss(miss_) {};
        
    // Setters
    void SetBeat(const Vector& beat_) { beat = beat_; }
    void SetMeet(const Vector& meet_) { meet = meet_; }
    void SetMiss(const Vector& miss_) { miss = miss_; }

    // Method for plot
    void plot() {
        FILE *gnuplotPipe = popen("gnuplot -persist", "w");
        
        int size = beat.size();
        vector<int> dates(size);
        dates[0] = -size/2;
        for (int i = 1; i < size; i++) {
        dates[i] = dates[i-1] + 1;
        }
        
        const char *title = "Avg CAAR for 3 groups";
        const char *xlabel = "Announcement Date";
        const char *ylabel = "Avg CAAR(%)";
        
        if (gnuplotPipe) {
            // Set title and labels
            fprintf(gnuplotPipe, "set title '%s'\n", title);
            fprintf(gnuplotPipe, "set xlabel '%s'\n", xlabel);
            fprintf(gnuplotPipe, "set ylabel '%s'\n", ylabel);
            
    
            // Set range for X and Y axis
            fprintf(gnuplotPipe, "set yrange [-25.0 :5.0]\n");
            fprintf(gnuplotPipe, "set xrange [%d:%d]\n", dates.front(), dates.back());
            fprintf(gnuplotPipe, "set grid\n");
    
            // Add a vertical red line at x = 0
            fprintf(gnuplotPipe, "set arrow from 0,graph(0,0) to 0,graph(1,1) nohead lc rgb 'red'\n");
    
            // Set up the plot command with data
            fprintf(gnuplotPipe, "plot '-' with lines title 'beat', '-' with lines title 'meet', '-' with lines title 'miss'\n");
    
            // Plot data for Beat, Meet, and Miss groups
            for (int i = 0; i < size; i++) {
                fprintf(gnuplotPipe, "%d %lf\n", dates[i], beat[i] * 100.0);
            }
            fprintf(gnuplotPipe, "e\n");
    
            for (int i = 0; i < size; i++) {
                fprintf(gnuplotPipe, "%d %lf\n", dates[i], meet[i] * 100.0);
            }
            fprintf(gnuplotPipe, "e\n");
    
            for (int i = 0; i < size; i++) {
                fprintf(gnuplotPipe, "%d %lf\n", dates[i], miss[i] * 100.0);
            }
            fprintf(gnuplotPipe, "e\n");
    
            // Flush and close the gnuplot pipe
            fflush(gnuplotPipe);
            fprintf(gnuplotPipe, "exit\n");
            pclose(gnuplotPipe);
        } else {
            std::cerr << "Could not open pipe to gnuplot.\n";
        }
   
    }
};


