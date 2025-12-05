float max_element(std::vector<float> l){
  float max=-10000;
  for (int i=0;i<l.size();i++)
  if (max<l[i]) max=l[i];
  return max;

}
