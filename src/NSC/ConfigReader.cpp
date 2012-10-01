#include "ConfigReader.h"
#include "SharedStateDistributor.h"

#include "EZLogger.h"

// temp error output
void print_out(const char* output)
{

	// make a basic logger bject to be shared
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
			EZLOGGERVLSTREAM(axter::log_always) << "The root element is not an array, Incorrect format." <<  std::endl;
		}
		else
		{
			root = value->AsArray();
			for (unsigned int i = 0; i < root.size(); i++)
			{	
				if (!root[i]->IsObject())
				{
					EZLOGGERVLSTREAM(axter::log_always) << "Array element: Object expected." <<  std::endl;
				}
				else
				{
					boost::shared_ptr<SharedStateAttributes> attributes = boost::shared_ptr<SharedStateAttributes>(new SharedStateAttributes);
					
					JSONObject object = root[i]->AsObject();
					if (object.find(L"id") != object.end())
					{
						if (object[L"id"]->IsString())
						{
							std::wstring ws = object[L"id"]->AsString();
							attributes->id = std::string( ws.begin(), ws.end() );
						}
						else
						{
							EZLOGGERVLSTREAM(axter::log_always) << "'id' : String expected." <<  std::endl;
							delete value;
							return;
						}
					}
					

					if (object.find(L"consumers") != object.end())
					{
						if (object[L"consumers"]->IsArray())
						{
							JSONArray consumerArray = object[L"consumers"]->AsArray();
							std::vector<std::string> consumers;
							for (unsigned int j = 0; j < consumerArray.size(); j++)
							{
								if (!consumerArray[j]->IsString())
								{
									EZLOGGERVLSTREAM(axter::log_always) << "'consumers' element : String expected." <<  std::endl;
									delete value;
									return;
								}
								std::wstring ws = consumerArray[j]->AsString();
								consumers.push_back(std::string( ws.begin(), ws.end()));
							}
						
							attributes->consumers = consumers;
						}
						else
						{
							EZLOGGERVLSTREAM(axter::log_always) << "'consumers' : Array expected." <<  std::endl;
							delete value;
							return;
						}
					}

					if (object.find(L"only_on_change") != object.end() )
					{
						if (object[L"only_on_change"]->IsBool())
							attributes->onlyOnChange = object[L"only_on_change"]->AsBool();
						else
						{
							EZLOGGERVLSTREAM(axter::log_always) << "'only_on_change' : Bool expected." <<  std::endl;
							delete value;
							return;
						}
					}

					if (object.find(L"reset_counter_on_restart") != object.end())
					{
						if (object[L"reset_counter_on_restart"]->IsBool())
							attributes->resetOnRestart = object[L"reset_counter_on_restart"]->AsBool();
						else
						{
							EZLOGGERVLSTREAM(axter::log_always) << "'reset_counter_on_restart' : Bool expected." <<  std::endl;
							delete value;
							return;
						}
					}

					if (object.find(L"interval") != object.end())
					{
					
						if (object[L"interval"]->IsNumber())
							attributes->interval = object[L"interval"]->AsNumber();
						else
						{
							EZLOGGERVLSTREAM(axter::log_always) << "'interval' : Number expected." <<  std::endl;
							delete value;
							return;
						}
					}

					if (object.find(L"delta") != object.end())
					{
					
						if (object[L"delta"]->IsBool())
							attributes->delta = object[L"delta"]->AsBool();
						else
						{
							EZLOGGERVLSTREAM(axter::log_always) << "'delta' : Bool expected." <<  std::endl;
							delete value;
							return;
						}
					}

					config.push_back(attributes);
				}
			}

		}

		delete value;
	}
	else 
	{
		EZLOGGERVLSTREAM(axter::log_always) << "Failed to parse config file." <<  std::endl;
	}
}

boost::shared_ptr<SharedStateAttributes> SharedStateConfigReader::Get(int i)
{

	if (i>= 0 && i < config.size())
		return config[i];
	else
		return boost::shared_ptr<SharedStateAttributes>();
}