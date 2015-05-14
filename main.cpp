/* -*- coding: utf-8; tab-width: 4 -*- */
/**
 * @file	main.cpp
 * @brief	segment_tree.hppの動作確認用コマンド
 * @author	Yasutaka SHINDOH / 新堂 安孝
 */

#include <cstdio>
#include "segment_tree.hpp"

int compare(const float& left,
			const float& right)
{
	if (left < right) return -1;
	if (left > right) return 1;
	return 0;
}

int main()
{
	float data[] = { 7.0, 2.0, 4.0, 3.0, 9.0, 8.0, 0.0, 6.0, 5.0, 1.0 };

	printf("INPUT:\n");
	for (size_t i(0); i < sizeof(data)/sizeof(data[0]); ++i) {
		printf("[%lu]\t%f\n", i, data[i]);
	}

	ys::SegmentTree<float>* tree = new ys::SegmentTree<float>();
	tree->prepare(data, sizeof(data)/sizeof(data[0]), compare);

	size_t queries[][2] = {{1, 2}, {2, 4}, {3, 7}, {8, 8}, {7, 9}};

	printf("OUTPUT:\n");
	for (size_t i(0); i < sizeof(queries)/sizeof(queries[0]); ++i) {
		std::printf("[%lu:%lu]\t%f\n", queries[i][0], queries[i][1],
					tree->get_value(queries[i][0], queries[i][1]));
	}

	delete tree;

	return 0;
}
