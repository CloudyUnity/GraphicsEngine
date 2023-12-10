#include "ModelParser.h"

bool ModelParser::ParseFile(const char* filename) 
{
	ifstream fin;
	char input = '0';

	fin.open(filename);

	if (fin.fail())
		return false;

	std::vector<float> posList;
	std::vector<float> texList;
	std::vector<float> norList;
	std::vector<int> faceList;

	while (fin.get(input))
	{
		if (input == 'f')
		{
			for (int i = 0; i < 3 && fin.peek() == ' '; i++)
			{
				int val;
				fin >> val;
				faceList.push_back(val);
				if (fin.peek() == '/')
					fin.get();

				fin >> val;
				faceList.push_back(val);
				if (fin.peek() == '/')
					fin.get();

				fin >> val;
				faceList.push_back(val);
			}
		}

		if (input != 'v')
			continue;

		fin.get(input);

		float val;
		int max = input == 't' ? 2 : 3;
		for (int i = 0; i < max; i++) {
			fin >> val;

			if (input == 't')
				texList.push_back(val);
			else if (input == 'n')
				norList.push_back(val);
			else if (input == ' ')
				posList.push_back(val);
			else
				break;
		}
	}

	fin.close();

	std::string path(filename);
	size_t lastSlashPos = path.find_last_of("\\/");
	size_t lastDotPos = path.find_last_of(".");
	std::string name = (lastSlashPos != std::string::npos) ? path.substr(lastSlashPos + 1, lastDotPos - lastSlashPos - 1) : path;
	std::string filepath = "../GraphicsEngine/Models/" + name + ".txt";

	std::ofstream out(filepath, std::ios::trunc);

	if (!out.is_open())
		return false;

	int vertexCount = faceList.size();
	out << "Vertex Count: " << vertexCount << std::endl << std::endl << "Data:" << std::endl;

	for (int i = 0; i < vertexCount; i += 3) {
		int posI = (faceList.at(i+0) - 1) * 3;
		int texI = (faceList.at(i+1) - 1) * 2;
		int norI = (faceList.at(i+2) - 1) * 3;

		out << posList.at(posI) << " " << posList.at(posI + 1) << " " << posList.at(posI + 2) << " ";
		out << texList.at(texI) << " " << texList.at(texI + 1) << " ";
		out << norList.at(norI) << " " << norList.at(norI + 1) << " " << norList.at(norI + 2) << std::endl;
	}

	out.close();

	return true;
}