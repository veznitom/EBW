#include <cstdint>
#include <string>

class Customs{  
public:
  class function{
    private:
      bool parse(){
        return false;
      }
    public:
      function(){}
  };

  enum Type:uint8_t{NUMBER,TEXT,FUNCTION,SYMBOL,LBRACK,RBRACK,UNDEF,END};
};


