
#include "VideoDecode.h"

VideoDecode::VideoDecode() {}

VideoDecode &VideoDecode::getInstance() {
  static VideoDecode instance;
    return instance;
}

void VideoDecode::test()
{

}

VideoDecode::~VideoDecode() {}
