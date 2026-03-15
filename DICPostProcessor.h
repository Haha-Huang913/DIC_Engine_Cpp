#pragma once
#include <vector>
#include <string>
#include "DICDataTypes.h"

class DICPostProcessor {
public:
	DICPostProcessor();

	// std::vector<DICPointResult> Fixtool(std::vector<DICPointResult>& fullFieldResults)与以下区别：return结果出来会再复制一遍，导致浪费内存，使用void以及引用传参可以防止复制，直接修改原件。
	void fixInvalidHoles(DICMeshField& fullFieldResults);	//8邻域坏点修复
};
