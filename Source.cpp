#include<iostream>
#include<fstream>
#include <string>
#include <time.h>
#include <algorithm>
#include <vector>
#include <ctime>


using namespace std;


char* substr(char* arr, int begin, int len)
{
	char* res = new char[len];
	for (int i = 0; i < len; i++)
		res[i] = *(arr + begin + i);
	res[len] = 0;
	return res;
}

void sleepcp(int milliseconds) // cross-platform sleep function
{
	clock_t time_end;
	time_end = clock() + milliseconds * CLOCKS_PER_SEC / 1000;
	while (clock() < time_end)
	{
	}
}

string convert(string str_in)
{
	for (int i = 0; i < str_in.length(); i++)
	{
		switch (str_in[i])
		{
		case '\\':
			str_in[i] = '\\';
			string str = "\\";
			str_in.insert(i, str);
			i++;
			break;
		}
	}
	return str_in;
}
string temp;

int main() {
	//libsvm\windows folder
	string lsvmw = "C:\\Users\\Dell\\Downloads\\libsvm-3.20\\libsvm-3.20\\windows\\";
	//target of batch for feature extraction; must be placed to ..\FERA-2015\C++ models\Release\ folder 
	string target_batch = "C:\\Users\\Dell\\Documents\\tere\\FERA-2015\\C++ models\\Release\\batch2.bat";
	//path to cohn-kanade-images folder
	string img_path = "E:\\Bakatoo\\Databases\\Cohn-Kanade_extended\\cohn-kanade-images";
	int emotion = 7;//SELECT WHAT TO TRAIN FOR
	/*
	How to:
	1) create batch file for extracting feature data(last 3 lines are currently garbage, will produce errors/should be deleted) - stage 0
	2) run batch2.bat from fera-2015/c++ models/release; will run featureextraction.exe for all necessary images
	3) create test files for classification, also creates batch.bat to lsvmw folder - stage 1
	4) run batch.bat from lsvmw folder, will train and predict for all different test cases
	4) process results - stage 2

	BEFORE running:
	emotion label .zip should be extracted on top of database
	files f.txt and f2.txt should be generated as follows:
	f.txt generated from cmd "dir /s/b/o:n /ad > f.txt" in cohn-kanade-images folder; gathers paths to all sets of img folders
	f2.txt generated from cmd in cohn-kanade-images folder w/ "dir /s/b/o:n *.txt > f2.txt"; gathers paths to all label files
	both should be in cohn-kanade-images folder

	stages to execute
	0 - create batch for extracting feature data
	1 - create test files for classification
	2 - process results after running the training batch
	*/
	int stage =1 ;

	//get path-s to emotion .txt files WORKS!!!!
	ifstream myReadFile;
	myReadFile.open(img_path + "\\f2.txt");
	char output[100];
	std::vector<string> vektor;
	if (myReadFile.is_open()){
		while (!myReadFile.eof()){
			myReadFile >> output;
			temp = output;
			temp = convert(temp);
			vektor.push_back(temp);
		}
		cout << "Got paths to emotion .txt files..." << endl;
	}

	//READ EXTRACTED FEATURE DATA
	std::vector<float> data;
	std::vector<int> labels;
	for (int i = 0; i < vektor.size(); i++){
		if (i >= 0){
			ifstream readfile;
			readfile.open(vektor[i]);
			char wat[100];

			if (readfile.is_open()){
				int j = 0;
				while (!readfile.eof()){
					readfile >> wat;
					//whole face
					if (j == 0){//label
						string a = substr(wat, 0, 1);
						if ((a == "7") || (a == "6") || (a == "5") || (a == "4") || (a == "3") || (a == "2") || (a == "1") || (a == "0")){
							int x = stoi(wat);//label of final emotion as integer!!!
							labels.push_back(0);
							labels.push_back(x);
						}
					}
					else if ((j > 2) && (j < 139)){//neutral emotion
						data.push_back(stof(wat));
					}
					else if ((j == 139) || (j == 140)){//bs data
						//!!!pointless!!!
					}
					else if (j > 140){
						//start of second emotion
						string temp = wat;
						if (!temp.empty()){
							data.push_back(stof(wat));
						}
					}
					j++;
				}
				cout << "feature data read from files..." << endl;
			}
			else{
				cout << "error" << endl;
			}
		}
	}
	

	float biggest = 0;
	cout << "labels:" << labels.size() << endl;
	for (int t = 0; t < labels.size(); t++){
		cout << t << ": " << to_string(labels[t]) << endl;
	}
	cout << "data:" << data.size() << endl;
	for (int t = 0; t < data.size(); t++){
		if (data[t] > biggest){
			biggest = data[t];
		}
	}
	cout << "data & labels extracted..." << endl;
	//-------------------------------------------------GOT ALL DATA!!--------------------------------------------------

	float trainingdata[654][136];

	float traininglabels[654];


	int count = 0;
	for (int t = 0; t < labels.size(); t++){
		if (labels[t] == emotion){
			traininglabels[t] = 1;
			count++;
		}
		else{
			traininglabels[t] = -1;
		}
	}

	//whole face
	int w = 0;
	for (int t = 0; t < 654; t++){
		for (int u = 0; u < 136; u++){
			trainingdata[t][u] = data[w] / biggest;
			trainingdata[t][u] = data[w];
			w++;
		}
	}
	cout << "sets and emotions separated..." << endl;
	//-------------------------------------------------------

	//Whole face
	float x[68];
	float y[68];
	int countx = 0;
	int county = 0;

	for (int i = 0; i < 136; i++){
		if (i % 2 == 0){
			x[countx] = int(trainingdata[1][i]);
			countx++;
		}
		else{
			y[county] = int(trainingdata[1][i]);
			county++;
		}
	}

	std::cout << "feature points separated..." << endl;

	//read out files for stats
	if (stage == 2){
		int an = 0;
		int co = 0;
		int di = 0;
		int fe = 0;
		int ha = 0;
		int sa = 0;
		int su = 0;

		std::cout << "emotion " << emotion << " exists in database " << count << " times";
		sleepcp(500);

		ifstream out;
		float resultlabels[654];
		int result = 0;
		int all = 0;
		for (int n = 0; n < 654; n++){
			//path to output files from svm
			out.open(lsvmw + "\\outU\\out" + to_string(n) + ".txt");
			char temp[100];
			out >> temp;
			int temp2;
			temp2 = stoi(temp);
			if (temp2 == traininglabels[n + 1]){
				result++;
			}
			else{
				if (labels[n + 1] == 1) an++;
				if (labels[n + 1] == 2) co++;
				if (labels[n + 1] == 3) di++;
				if (labels[n + 1] == 4) fe++;
				if (labels[n + 1] == 5) ha++;
				if (labels[n + 1] == 6) sa++;
				if (labels[n + 1] == 7) su++;
			}
			n++;
			all++;
			out.close();
		}

		//results
		std::cout << "Upper | Correct: " << result << "; %: " << (float)result / (float)all << endl;
		std::cout << "All samples#: " << all << " | angry: " << 100 * (float)an / (float)all << "% | contempt: " << 100 * (float)co / (float)all << "% | disgust: " << 100 * (float)di / (float)all << "% | fear: " << 100 * (float)fe / (float)all << "% | happy: " << 100 * (float)ha / (float)all << "% | sadness: " << 100 * (float)sa / (float)all << "% | surprise: " << 100 * (float)su / (float)all << "%" << endl;
		
		for (int n = 0; n < 654; n++){
			//path to output files from svm
			out.open(lsvmw + "\\outL\\out" + to_string(n) + ".txt");
			char temp[100];
			out >> temp;
			int temp2;
			temp2 = stoi(temp);
			if (temp2 == traininglabels[n + 1]){
				result++;
			}
			else{
				if (labels[n + 1] == 1) an++;
				if (labels[n + 1] == 2) co++;
				if (labels[n + 1] == 3) di++;
				if (labels[n + 1] == 4) fe++;
				if (labels[n + 1] == 5) ha++;
				if (labels[n + 1] == 6) sa++;
				if (labels[n + 1] == 7) su++;
			}
			n++;
			all++;
			out.close();
		}

		//results
		std::cout << "Lower | Correct: " << result << "; %: " << (float)result / (float)all << endl;
		std::cout << "All samples#: " << all << " | angry: " << 100 * (float)an / (float)all << "% | contempt: " << 100 * (float)co / (float)all << "% | disgust: " << 100 * (float)di / (float)all << "% | fear: " << 100 * (float)fe / (float)all << "% | happy: " << 100 * (float)ha / (float)all << "% | sadness: " << 100 * (float)sa / (float)all << "% | surprise: " << 100 * (float)su / (float)all << "%" << endl;
		sleepcp(10000);
	}

	//CREATE TEST FILES FOR CLASSIFICATION
	if (stage == 1){
		// libsvm-3.20\windows path

		ofstream batch;
		//name of the batch file for svm
		batch.open(lsvmw + "batch.bat");
		for (int n = 0; n < 654; n++){
			//batch file
			batch << "svm-train.exe -s 0 -t 0 -c 10 -g 0.5 -e 0.1 testU\\test" << to_string(n) << ".txt" << endl;
			batch << "svm-predict.exe katseU\\katse" << to_string(n) << ".txt testU\\test" << to_string(n) << ".txt.model outU\\out" << to_string(n) << ".txt" << endl;

			batch << "svm-train.exe -s 0 -t 0 -c 10 -g 0.5 -e 0.1 testL\\test" << to_string(n) << ".txt" << endl;
			batch << "svm-predict.exe katseL\\katse" << to_string(n) << ".txt testL\\test" << to_string(n) << ".txt.model outL\\out" << to_string(n) << ".txt" << endl;

			ofstream ofs;
			//generate test files
			ofs.open(lsvmw + "testU\\test" + to_string(n) + ".txt");

			ofstream oft;
			//generate test files
			oft.open(lsvmw + "testL\\test" + to_string(n) + ".txt");

			int temp[654];
			int templ[654];
			for (int t = 0; t < 654; t++){

				int temo = 0;
				if (t == n){
					ofstream ofb;//for upper face
					ofstream ofc;//for lower face
					//add 1 line for testing to katse.txt instead
					ofb.open(lsvmw + "katseU\\katse" + to_string(n) + ".txt");
					if (traininglabels[t + 1] == 1){
						ofb << "+1 ";
					}
					else{
						ofb << "-1 ";
					}
					ofc.open(lsvmw + "katseL\\katse" + to_string(n) + ".txt");
					if (traininglabels[t + 1] == 1){
						ofc << "+1 ";
					}
					else{
						ofc << "-1 ";
					}
					//upper face
					float biggest_dist = 0;
					float biggest_dist_l = 0;
					for (int u = 0; u < 136; u++){
						float dist = 0;
						if (u <= 7){
							dist = sqrt((((trainingdata[t][u] - trainingdata[t + 1][u])*(trainingdata[t][u] - trainingdata[t + 1][u]) + (trainingdata[t][u + 1] - trainingdata[t + 1][u + 1])*(trainingdata[t][u + 1] - trainingdata[t + 1][u + 1]))));
						}
						else if ((u >= 26) && (u <= 71)){
							dist = sqrt((((trainingdata[t][u] - trainingdata[t + 1][u])*(trainingdata[t][u] - trainingdata[t + 1][u]) + (trainingdata[t][u + 1] - trainingdata[t + 1][u + 1])*(trainingdata[t][u + 1] - trainingdata[t + 1][u + 1]))));
						}
						else{
							//lower part
							dist = sqrt((((trainingdata[t][u] - trainingdata[t + 1][u])*(trainingdata[t][u] - trainingdata[t + 1][u]) + (trainingdata[t][u + 1] - trainingdata[t + 1][u + 1])*(trainingdata[t][u + 1] - trainingdata[t + 1][u + 1]))));
						}
						if (dist > biggest_dist){
							biggest_dist = dist;
						}
						temp[t] = biggest_dist;
					}

					for (int u = 0; u < 136; u++){
						//features of upper face only
						float dist = 0;
						if (u <= 7){
							dist = sqrt((((trainingdata[t][u] - trainingdata[t + 1][u])*(trainingdata[t][u] - trainingdata[t + 1][u]) + (trainingdata[t][u + 1] - trainingdata[t + 1][u + 1])*(trainingdata[t][u + 1] - trainingdata[t + 1][u + 1]))))/temp[t];
							ofb << to_string(temo) << ":" << dist << " ";
						}
						else if ((u >= 26)&&(u <=71)){
							dist = sqrt((((trainingdata[t][u] - trainingdata[t + 1][u])*(trainingdata[t][u] - trainingdata[t + 1][u]) + (trainingdata[t][u + 1] - trainingdata[t + 1][u + 1])*(trainingdata[t][u + 1] - trainingdata[t + 1][u + 1]))))/temp[t];
							ofb << to_string(temo) << ":" << dist << " ";
						}
						else{
						//lower face
							dist = sqrt((((trainingdata[t][u] - trainingdata[t + 1][u])*(trainingdata[t][u] - trainingdata[t + 1][u]) + (trainingdata[t][u + 1] - trainingdata[t + 1][u + 1])*(trainingdata[t][u + 1] - trainingdata[t + 1][u + 1])))) / temp[t];
							ofc << to_string(temo) << ":" << dist << " ";
						}
						//if (dist > 0){
							
						//}
						
						u++;
						temo++;
					}
					//cout << "biggest distance is: " << biggest_dist << endl;
					/*whole face
					for (int u = 0; u < 136; u++){
						//if (trainingdata[t][u]> 0.1){//pointless
						if (){
							float dist;
							dist = 8 * sqrt((((trainingdata[t][u] - trainingdata[t + 1][u])*(trainingdata[t][u] - trainingdata[t + 1][u]) + (trainingdata[t][u + 1] - trainingdata[t + 1][u + 1])*(trainingdata[t][u + 1] - trainingdata[t + 1][u + 1]))));

							if (dist > 0.1){
								ofb << to_string(temo) << ":" << dist << " ";
								if (dist > 1){
								}
							}
						}
						u++;
						temo++;
					}
					*/
				}
				else{
					if (traininglabels[t + 1] == 1){
						ofs << "+1 ";
					}
					else{
						ofs << "-1 ";
					}

					if (traininglabels[t + 1] == 1){
						oft << "+1 ";
					}
					else{
						oft << "-1 ";
					}

					for (int u = 0; u < 136; u++){
						
						float dist = 0;
						if (u <= 7){
							dist = sqrt((((trainingdata[t][u] - trainingdata[t + 1][u])*(trainingdata[t][u] - trainingdata[t + 1][u]) + (trainingdata[t][u + 1] - trainingdata[t + 1][u + 1])*(trainingdata[t][u + 1] - trainingdata[t + 1][u + 1])))) / temp[t];
							ofs << to_string(temo) << ":" << dist << " ";
						}
						else if ((u >= 26) && (u <= 71)){
							dist = sqrt((((trainingdata[t][u] - trainingdata[t + 1][u])*(trainingdata[t][u] - trainingdata[t + 1][u]) + (trainingdata[t][u + 1] - trainingdata[t + 1][u + 1])*(trainingdata[t][u + 1] - trainingdata[t + 1][u + 1])))) / temp[t];
							ofs << to_string(temo) << ":" << dist << " ";
						}
						else{
							//lower
							dist = sqrt((((trainingdata[t][u] - trainingdata[t + 1][u])*(trainingdata[t][u] - trainingdata[t + 1][u]) + (trainingdata[t][u + 1] - trainingdata[t + 1][u + 1])*(trainingdata[t][u + 1] - trainingdata[t + 1][u + 1])))) / temp[t];
							oft << to_string(temo) << ":" << dist << " ";
						}
					//	if (dist > 0){
							
					//	}
						u++;
						temo++;
					}
					ofs << endl;
					oft << endl;
				}
				t++;
			}
			n++;
		}
	}

	//CREATE BATCH FILE FOR FEATURE EXTRACTION
	if (stage == 0){

		ifstream myReadFile, myReadFile2;
		ofstream batch;
		batch.open(target_batch);//target batch file

		myReadFile.open(img_path + "\\f.txt");

		char output[100];
		string myout0, myout1, myout2, myout3, out;
		myout0 = "FeatureExtraction.exe -fdir \"";
		myout2 = "\" -of \"";
		//path to images folder
		myout3 = img_path;
		int counter = 0;

		if (myReadFile.is_open()) {
			while (!myReadFile.eof()) {
				myReadFile >> output;
				cout << output << endl;
				char* temp = "";
				char* temp2 = "";
				char* temp3 = "";
				string x = "";
				temp = substr(output, 61, 69);

				temp2 = substr(temp, 0, 4);
				temp3 = substr(temp, 5, 3);
				x.append(temp2);
				x.append("_");
				x.append(temp3);
				x.append("_");

				//open .txt with paths to emotion .txt files

				myReadFile2.open(img_path + "\\f2.txt");
				if (myReadFile2.is_open()){
					while (!myReadFile2.eof()){
						char output2[100];
						myReadFile2 >> output2;

						char* xtemp = "";
						char* xtemp2 = "";
						char* xtemp3 = "";
						string xx = "";
						xtemp = substr(output2, 61, 69);

						xtemp2 = substr(xtemp, 0, 4);
						xtemp3 = substr(xtemp, 5, 3);
						xx.append(xtemp2);
						xx.append("_");
						xx.append(xtemp3);
						xx.append("_");
						if (!xx.compare(x)){
							//cout << "--------------------------------------" << endl;
							//cout << myout0 << output << myout2 << output2 << "\"" << endl;
							out = myout0 + output + myout2 + output2 + "\"";
							batch << out << endl;
							//WORKS
						}

					}
					myReadFile2.close();
				}
				counter++;
			}
		}
		batch.close();
		myReadFile.close();
		sleepcp(2000);
	}


	return 0;
}
