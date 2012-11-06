#include "ConfigReader.h"
#include "State.h"
#include "CommandController.h"

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

void StateConfigReader::ReadConfig(std::string file)
{
	std::ifstream ifs(file);
	std::string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	
	JSONValue *value = JSON::Parse(str.c_str());

	if (value != NULL)
	{
		JSONObject root;
		if (value->IsObject() == false)
		{
			EZLOGGERVLSTREAM(axter::log_always) << "The root element is not an object, Incorrect format." <<  std::endl;
		}
		else
		{
			root = value->AsObject();
			if (root.find(L"aliases") != root.end())
			{
				_ReadAliases(root[L"aliases"]);
			}
			if (root.find(L"outputs") != root.end())
			{
				_ReadOutputs(root[L"outputs"]);
			}
			if (root.find(L"inputs") != root.end())
			{
				_ReadInputs(root[L"inputs"]);
			}
		}
		delete value;
	}
	else 
	{
		EZLOGGERVLSTREAM(axter::log_always) << "Failed to parse config file." <<  std::endl;
	}

}

void StateConfigReader::_ReadAliases(JSONValue* value)
{
	if (value->IsObject())
	{
		JSONObject aliases = value->AsObject();
		
		for(auto iterator = aliases.begin(); iterator != aliases.end(); iterator++) 
		{
			std::string key = _WStringToString(iterator->first);
			JSONValue* v = iterator->second;
			if (v->IsString())
			{
				std::string val = _WStringToString(v->AsString());
				m_Aliases[key] = val;
			}
			else
			{
				EZLOGGERVLSTREAM(axter::log_always) << "Alias format is 'String' : 'String' " <<  std::endl;
			}
		}
	}
	else
	{
		EZLOGGERVLSTREAM(axter::log_always) << "Aliases should be formatted as a JSON Object" <<  std::endl;
	}
}

void StateConfigReader::_ReadInputs(JSONValue *value)
{
	if (value != NULL)
	{
		JSONArray root;
		if (value->IsArray() == false)
		{
			EZLOGGERVLSTREAM(axter::log_always) << "Array of input states expected, Incorrect format." <<  std::endl;
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
					JSONObject object = root[i]->AsObject();

					JSONValue* id = (object.find(L"id") != object.end())?  object[L"id"] : NULL;
					JSONValue* from = (object.find(L"from") != object.end())?  object[L"from"] : NULL;
					JSONValue* route = (object.find(L"route") != object.end())?  object[L"route"] : NULL;

					if (id != NULL && from != NULL)
					{
						auto attributes = boost::shared_ptr<CommandAttributes>(new CommandAttributes);
						std::wstring ws = object[L"id"]->AsString();
						attributes->id = std::string( ws.begin(), ws.end() );

						attributes->from  = _ReadSources(from);
						attributes->route = _ReadSources(route);

						m_CommandConfig.push_back(attributes);
					}
					else
					{
						EZLOGGERVLSTREAM(axter::log_always) << "Input element: id and source expected." <<  std::endl;
					}
				}
			}
		}
	}
}

void StateConfigReader::_ReadOutputs(JSONValue *value)
{
	if (value != NULL)
	{
		JSONArray root;
		if (value->IsArray() == false)
		{
			EZLOGGERVLSTREAM(axter::log_always) << "Array of distribution states expected, Incorrect format." <<  std::endl;
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
					boost::shared_ptr<StateAttributes> attributes = boost::shared_ptr<StateAttributes>(new StateAttributes);

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
						attributes->consumers =  _ReadSources(object[L"consumers"]);
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
		//delete value;
	}
}

std::vector<std::string> StateConfigReader::_ReadSources(JSONValue* value)
{
	std::vector<std::string> sources;
	if (value && value->IsArray())
	{
		JSONArray sourceArray = value->AsArray();
		
		for (unsigned int j = 0; j < sourceArray.size(); j++)
		{
			if (!sourceArray[j]->IsString())
			{
				EZLOGGERVLSTREAM(axter::log_always) << "'source' element : String expected." <<  std::endl;
			}
			else
			{
				std::wstring ws = sourceArray[j]->AsString();
				std::string source = std::string( ws.begin(), ws.end());

				// check if this is an alias, if so insert the fully qualified address
				auto iterator = m_Aliases.find(source);
				if (iterator != m_Aliases.end())
					sources.push_back(m_Aliases[source]);
				else
					sources.push_back(source);
			}
		}
	}
	else
	{
		EZLOGGERVLSTREAM(axter::log_always) << "'sources' : Array expected." <<  std::endl;
	}
	return sources;
}

boost::shared_ptr<StateAttributes> StateConfigReader::GetStateAttribute(int i)
{

	if (i>= 0 && i < config.size())
		return config[i];
	else
		return boost::shared_ptr<StateAttributes>();
}

boost::shared_ptr<CommandAttributes> StateConfigReader::GetCommandAttribute(int i)
{

	if (i>= 0 && i < m_CommandConfig.size())
		return m_CommandConfig[i];
	else
		return boost::shared_ptr<CommandAttributes>();
}


