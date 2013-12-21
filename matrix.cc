// File: matrix.cc
// Date: Sat Dec 21 17:31:06 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#include <boost/numeric/mtl/mtl.hpp>
#include "matrix.hh"
#include "geometry.hh"

typedef mtl::matrix::dense2D<real_t> mtlM;
using namespace std;

ostream& operator << (std::ostream& os, const Matrix & m) {
	os << "[" << m.w << " " << m.h << "] :" << endl;
	REP(i, m.h) REP(j, m.w)
		os << m.get(i, j) << (j == m.w - 1 ? "\n" : ", ");
	return os;
}

Matrix Matrix::transpose() const {
	Matrix ret(h, w);
	REP(i, h) REP(j, w)
		ret.get(j, i) = val[i][j];
	return move(ret);
}

Matrix Matrix::prod(const Matrix & r) const {
	m_assert(w == r.h);
	const Matrix transp(r.transpose());
	Matrix ret(r.w, h);
	REP(i, h) REP(j, r.w) REP(k, w)
		ret.get(i, j) += val[i][k] * transp.get(j, k);
	return move(ret);
}

bool Matrix::inverse(Matrix &ret) const {
	m_assert(w == h && w == ret.w && ret.w == ret.h);

	mtlM input(h, w);
	REP(i, h) REP(j, w) input(i, j) = get(i, j);
	mtlM inverse(h, w);
	try {
		inv(input, inverse);
	} catch (...) {
		return false;
	}

	REP(i, h) REP(j, w)
		ret.get(i, j) = inverse(i, j);

	return true;
}

bool Matrix::solve_overdetermined(Matrix & x, const Matrix & b) const {
	m_assert(h >= w);			// check overdetermined
	Matrix mt = transpose();
	Matrix mtm = mt.prod(*this);
	Matrix inverse(mtm.w, mtm.h);
	if (!mtm.inverse(inverse))		// TODO judge determinant threshold 0.001
		return false;
	x = inverse.prod(mt).prod(b);
	return true;
}

bool Matrix::SVD(Matrix& u, Matrix& s, Matrix& v) const {
	mtlM A(h, w);
	REP(i, h) REP(j, w) A(i, j) = get(i, j);
	mtlM l(h, h), m(h, w), r(w, w);
	boost::tie(l, m, r) = svd(A, 1.e-6);
	/*
	 *cout << "done" << endl;
	 *cout << l << endl << m << endl   << r << endl;
	 *mtlM result(h, w);
	 *mult(m, trans(r), result);
	 *mult(l, result, result);
	 *cout <<  result << endl;
	 */

	REP(i, h) REP(j, h)
		u.get(i, j) = l(i, j);
	REP(i, h) REP(j, w)
		s.get(i, j) = m(i, j);
	REP(i, w) REP(j, w)
		v.get(i, j) = r(i, j);

	return true;
}

void Matrix::normrot() {
	m_assert(w == 3);
	Vec p(val[0][0], val[1][0], val[2][0]);
	Vec q(val[0][1], val[1][1], val[2][1]);
	Vec r(val[0][2], val[1][2], val[2][2]);
	p.normalize();
	q.normalize();
	r.normalize();
	Vec vtmp = p.cross(q);
	real_t dist = (vtmp - r).mod();
	if (dist > 1e-6)
		r = vtmp;
	val[0][0] = p.x, val[1][0] = p.y, val[2][0] = p.z;
	val[0][1] = q.x, val[1][1] = q.y, val[2][1] = q.z;
	val[0][2] = r.x, val[1][2] = r.y, val[2][2] = r.z;
}

real_t Matrix::sqrsum() const {
	m_assert(w == 1);
	real_t sum = 0;
	REP(i, h)
		sum += sqr(val[i][0]);
	return sum;
}

Matrix Matrix::col(int i) const {
	m_assert(i < w);
	Matrix ret(1, h);
	REP(j, h)
		ret.get(j, 0) = get(j, i);
	return move(ret);
}

Matrix Matrix::I(int k) {
	Matrix ret(k, k);
	REP(i, k)
		ret.get(i, i) = 1;
	return move(ret);
}
