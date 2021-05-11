#include "garaga.hpp"
#include <iostream>
#include <stdexcept>

int main(int argc, char **argv)
{
  try
  {
    garaga s;
    return s.exec();
  } catch (std::exception &e)
  {
    std::cerr << e.what() << std::endl;
    return 1;
  }
}
