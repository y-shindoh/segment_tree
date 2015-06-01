/* -*- coding: utf-8; tab-width: 4 -*- */
/**
 * @file	segment_tree.hpp
 * @brief	segment treeのテンプレート
 * @author	Yasutaka SHINDOH / 新堂 安孝
 * @note	http://www.slideshare.net/sndtkrh/ss-47669151 を参考に実装した。
 * @note	遅延評価の機能は組み込んでいない。
 */

#ifndef	__SEGMENT_TREE_HPP__
#define	__SEGMENT_TREE_HPP__ "segment_tree.hpp"

#include <cstddef>
#include <cassert>
#include <vector>

namespace ys
{
	/**
	 * @class	segment treeのテンプレート
	 * @note	インスタンス・フィールドに依存した最適値を算出する。
	 */
	template<typename TYPE>
	class SegmentTree
	{
	private:

		size_t m_;						///< 実データの総数
		size_t n_;						///< 葉ノードの総数 (2の冪数)
		std::vector<TYPE> data_;		///< 木データ (二分ヒープ同様、配列で表現)

		/**
		 * 最適値算出用のデータ比較関数
		 * 返却値が0以下:	第一引数が最適値
		 * 返却値が1以上:	第二引数が最適値
		 */
		int (* compare_)(const TYPE&, const TYPE&);

		/**
		 * 木の葉ノードに値を設定 (内部ノードへの影響はなし)
		 * @param[in]	i	外部配列のインデックス
		 * @param[in]	value	設定する値
		 * @note	計算量は Θ(1) となる。
		 */
		void
		set_leaf_simply(size_t i,
						const TYPE& value)
			{
				assert(i < m_);

				i += n_;
				data_[i] = value;
			}

		/**
		 * 木の内部ノードの値を更新 (葉ノードへの影響はなし)
		 * @param[in]	i	外部配列のインデックス
		 * @note	計算量は Θ(log n) となる。
		 */
		void
		update(size_t i)
			{
				assert(i < m_);

				i += n_;
				size_t e(m_ + n_ - 1);
				size_t h, k;
				while (0 < i) {
					i /= 2;
					h = i * 2;
					k = h + 1;
					if (k <= e && 0 < compare_(data_[h], data_[k])) h = k;
					data_[i] = data_[h];
					e /= 2;
				}
			}

		/**
		 * 指定範囲内の最適値を取得 ( @a get_value の補助関数)
		 * @param[in]	from	指定範囲の始点インデックス
		 * @param[in]	to	指定範囲の終点インデックス
		 * @param[in]	index	後述の @a left, @a right の最適値を持つインデックス
		 * @param[in]	left	探索範囲の始点インデックス
		 * @param[in]	right	探索範囲の終点インデックス
		 * @note	返却値 @a 0 は取得失敗。
		 * @note	インデックスは木のもの。1始まり。
		 * @note	計算量は O(k + log n) となる。ただし k はヒットした範囲の数。
		 */
		size_t
		get_value_sub(size_t from,
					  size_t to,
					  size_t index,
					  size_t left,
					  size_t right) const
			{
				if (right < from || to < left) return 0;		// 範囲外
				if (from <= left && right <= to) return index;	// 適正値候補
				const size_t center = (left + right) / 2;
				size_t i = get_value_sub(from, to, index * 2, left, center);
				size_t j = get_value_sub(from, to, index * 2 + 1, center + 1, right);
				if (!j) return i;
				if (!i) return j;
				if (0 >= compare_(data_[i], data_[j])) return i;
				return j;
			}

	public:

		/**
		 * コンストラクタ
		 */
		SegmentTree()
			: m_(0), n_(0), compare_(0)
			{
				;
			}

		/**
		 * 木の構築
		 * @param[in]	data	入力データの外部配列
		 * @param[in]	n	引数 @a data の持つ要素数
		 * @param[in]	データを比較する関数のポインタ (see @a compare_ )
		 * @note	計算量は Θ(n log n) となる。
		 */
		void
		prepare(const TYPE* data,
				size_t n,
				int (* compare)(const TYPE&, const TYPE&))
			{
				assert(data);
				assert(n);
				assert(compare);

				m_ = n;
				compare_ = compare;

				// メモリ領域
				n_ = 1;
				while (n_ < n) n_ *= 2;
				data_.resize(m_ + n_);

				// 葉ノード
				for (size_t i(0); i < n; ++i) {
					set_leaf_simply(i, data[i]);
				}
				// 内部ノード (手抜き)
				for (size_t i(0); i < n; i += 2) {
					update(i);
				}
			}

		/**
		 * 値を葉ノードに設定
		 * @param[in]	i	外部配列のインデックス
		 * @param[in]	value	設定する値
		 * @note	計算量は Θ(log n) となる。
		 */
		void
		set_leaf(size_t i,
				 const TYPE& value)
			{
				assert(i < m_);

				// 葉ノード
				set_leaf_simply(i, value);
				// 内部ノード
				update(i);
			}

		/**
		 * 指定範囲の最適値を取得
		 * @param[in]	from	指定範囲の始点インデックス
		 * @param[in]	to	指定範囲の終点インデックス
		 * @return	最適値
		 * @note	インデックスは外部配列のもの。
		 * @note	計算量は O(k + log n) となる。ただし k はヒットした範囲の数。
		 */
		TYPE
		get_value(size_t from,
				  size_t to) const
			{
				assert(from <= to);
				assert(to < m_);

				size_t i =  get_value_sub(from + 1, to + 1, 1, 1, n_);
				assert(i);

				return data_[i];
			}

		/**
		 * 木データの直接アクセス
		 * @param[in]	i	木データを表現する配列のインデックス
		 * @return	木データの該当箇所を指すポインタ
		 * @note	不正な引数に対して @a 0 を返却する。
		 * @note	計算量は Θ(1) となる。
		 */
		TYPE*
		get_raw_data(size_t i)
			{
				if (i < m_ + n_) return &data_[i];
				return 0;
			}
	};
};

#endif	// __SEGMENT_TREE_HPP__
