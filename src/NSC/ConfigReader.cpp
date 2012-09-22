#include "ConfigReader.h"

#include "SharedStateDistributor.h"


// temp error output
void print_out(const char* output)
{
	std::ofstream myfile;
	myfile.open ("errors.txt",std::ios::app);
	myfile << output <<std::endl;
	myfile.close();
}

void SharedStateConfigReader::ReadConfig(std::string file)
{

	std::ifstream ifs(file);
	std::string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	
	JSONValue *value = JSON::Parse(str.c_str());
	
	if (value != NULL)
	{
		JSONArray root;
		if (value->IsArray() == false)
		{
			print_out("The root element is not an array, Incorrect format.\r\n");
		}
		else
		{
			root = value->AsArray();
			for (unsigned int i = 0; i < root.size(); i++)
			{	
				if (!root[i]->IsObject())
				{
				}
				else
				{
					boost::shared_ptr<SharedStateAttributes> attributes = boost::shared_ptr<SharedStateAttributes>(new SharedStateAttributes);
					
					JSONObject object = root[i]->AsObject();
					if (object.find(L"id") != object.end() && object[L"id"]->IsString())
					{
						std::wstring ws = object[L"id"]->AsString();
						attributes->id = std::string( ws.begin(), ws.end() );
					}

					if (object.find(L"consumers") != object.end() && object[L"consumers"]->IsArray())
					{
						JSONArray consumerArray = object[L"consumers"]->AsArray();
						std::vector<std::string> consumers;
						for (unsigned int j = 0; j < consumerArray.size(); j++)
						{
							if (!consumerArray[j]->IsString())
							{
								//log
							}
							std::wstring ws = consumerArray[j]->AsString();
							consumers.push_back(std::string( ws.begin(), ws.end()));
						}
						
						attributes->consumers = consumers;
					}

					if (object.find(L"only_on_change") != object.end() && object[L"only_on_change"]->IsBool())
					{
						attributes->onlyOnChange = object[L"only_on_change"]->AsBool();
					}

					if (object.find(L"reset_counter_on_restart") != object.end() && object[L"reset_counter_on_restart"]->IsBool())
					{
						attributes->resetOnRestart = object[L"reset_counter_on_restart"]->AsBool();
					}

					if (object.find(L"interval") != object.end() && object[L"interval"]->IsNumber())
					{
					
						attributes->interval = object[L"interval"]->AsNumber();
					}

					config.push_back(attributes);
				}
			}

		}

		delete value;
	}
	else 
	{
		print_out("Failed to parse config file\r\n");
	}
}

boost::shared_ptr<SharedStateAttributes> SharedStateConfigReader::Get(int i)
{

	if (i>= 0 && i < config.size())
		return config[i];
	else
		return boost::shared_ptr<SharedStateAttributes>();
}