#include <stdio.h>
#include <limits.h>
#include <iostream>
#include <vector>

using namespace std;


 vector<double> kmeans(int* data, int num_data) {
    int min = INT_MAX,
        max = INT_MIN;
    if(!num_data)
        return {0.0, 0.0, 0.0};
    for(int x = 0; x< num_data; x++) {
        if(data[x]<min) 
            min = data[x];
        if(data[x]>max) 
            max = data[x];
    }
    double dmin = (double) min, dmax = (double) max;
    // vector<double> centroids = {(double) min, (double)(max-min) / 2.0, (double)max};
    // vector<double> centroids = {(double)min + (double)(max-min) / 4.0, ((double)min + (double)(max-min)/ 2.0), (double)min + (double)(max-min) * 0.75};
    vector<double> centroids = {dmin , dmin + (dmax - dmin) / 2.6, dmin + (dmax - dmin) * 0.68};
    // vector<double> centroids = {dmin , dmin + (dmax - dmin) * 0.3, dmin + (dmax - dmin) * 0.75};
    // vector<double> centroids = {((double)max) / 4.0, ((double)max * 2.0) / 4.0, ((double)max * 3.0) / 4.0};
    // vector<double> centroids = {(max-min) / 4.0, ((max-min) * 2.0) / 4.0, ((max-min) * 3.0) / 4.0};

    while(true) {
        double means[3] = {0};
        double counts[3] = {0};
        for(int x = 0; x<num_data; x++) {
            int min_dist = INT_MAX, index = 0;
            for(int y = 0; y<3; y++) {
                int dist = abs(data[x] - centroids[y]);
                if(dist<min_dist) {
                    min_dist = dist;
                    index = y;
                }
            }
            means[index]+=(double)data[x];
            counts[index]++;
        }
        int zeros = 0;
        for(int x = 0; x<3; x++) {
            if(counts[x] == 0 || abs((means[x] / counts[x]) - centroids[x]) == 0) {
                zeros++;
                continue;
            }
            centroids[x] = means[x] / counts[x]; 
        }
        if(zeros == 3)
            break;
    }
    return centroids;
}

//YOUR CODE
//input:
// int[] data : array of data including previous data so far, with length of num_data.
// num_data : number of data above
int predict(int* data, int num_data) {
    //your new data should be data[num_data-1];
    int value = data[num_data-1];

     vector<double> windows = kmeans(data, num_data-1);

    double border1 = (windows[0]+windows[1]) / 2.0,
           border2 = (windows[1]+windows[2]) / 2.0;
    if(value<=border1)return 0;
    if(value <= border2)return 1;
    else return 2;
}



//////////////////////////////////////////////////////////
//////////////////DO NOT MODIFY BELOW/////////////////////
//////////////////////////////////////////////////////////
int main(void){
    float average_score = 0;
    for(int f=1; f <= 4; f++){
        char filename[10];
        sprintf(filename, "data%d.txt", f);
        FILE* fp = fopen(filename, "r");
        
        int N;
        fscanf(fp, "%d", &N);
        int data[N];
        int score = 0;
        int min = INT_MAX, max = INT_MIN;
        for(int i=0; i < N; i++){
            int d, label;
            fscanf(fp, "%d %d", &d, &label);
            data[i] = d;
            if(d>max) 
                max = d;
            if(d<min) 
                min = d;
            int pred = predict(data, i+1);
            // cout<<"pred: "<<pred<<" but label was: "<<label<<endl;
            if(pred == label)score++;
        }
        // cout<<"glob min: "<<min<<" glob max: "<<max<<" diff: "<<(max-min)<<endl;
        printf("round %d: your score: %f\n",f, score / float(N));
        average_score += score/float(N);
        //score the prediction
        fclose(fp);
    }
    average_score /= 4;
    printf("average score: %f\n", average_score);
}