class DAEFile
{

public:

  DAEFile() { };

  void load(const std::string& path)
  {
    bool loadOkay = doc_.LoadFile(path.c_str());

    if (loadOkay)
    {
      std::clog << "file loaded" << std::endl;
    }
    else
    {
      std::clog << "Failed to load file" << std::endl;
    }    
  }

  GeometryList geometries()
  {
    std::clog << "checking geometries" << std::endl;
    if (geometries_.empty())
    {
      std::clog << "loading geometries" << std::endl;
      TiXmlElement* element = doc_.RootElement()->FirstChild("library_geometries")->ToElement();

      for(TiXmlNode* child = element->FirstChild(); child != 0; child = child->NextSibling())
      {
        Geometry geometry(child->ToElement());
        geometries_.push_back(geometry);
      }
      
      std::clog << "loaded geometries" << std::endl;
    }
    
    return geometries_;
  }

private:
  
  TiXmlDocument doc_;
  GeometryList geometries_;

};
