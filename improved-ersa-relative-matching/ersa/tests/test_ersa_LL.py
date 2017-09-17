"""Unit Tests for ersa/ersa_LL.py"""
#   Copyright (c) 2015 by
#   Richard Munoz <rmunoz@nygenome.org>
#   Jie Yuan <jyuan@nygenome.org>
#   Yaniv Erlich <yaniv@nygenome.org>
#
#   All rights reserved
#   GPL license

from ersa.ersa_LL import *
from ersa.ersa_LL import _n_to_ord, _n_to_w
from ersa.parser import get_pair_dict
import pytest
from math import log
from scipy.stats import poisson

class TestBackground:
    t = 1
    theta = 2
    lambda_ = 3
    B = Background(t, theta, lambda_)

    def test_init(self):
        assert self.B.t == self.t
        assert self.B.theta == self.theta
        assert self.B.lambda_ == self.lambda_

    def test_Fp(self):
        with pytest.raises(AssertionError):
            self.B._Fp(self.t - 1)
        assert self.B._Fp(self.t) == -log(2)
        assert self.B._Fp(self.t + 1) == -1 / self.theta - log(self.theta)

    def test_Sp(self):
        assert self.B._Sp([5]) == self.B._Fp(5)
        assert self.B._Sp([5, 10]) == self.B._Fp(5) + self.B._Fp(10)

    def test_Np(self):
        for n in range(10):
            Np_obs = self.B._Np(n)
            Np_exp = log(poisson.pmf(n, self.lambda_))
            assert abs(Np_exp - Np_obs) < (10 ** -8)

    def test_LL(self):
        s = [4, 5, 6, 7, 8, 10]
        n = len(s)
        assert self.B.LL(n, s) == self.B._Np(n) + self.B._Sp(s)


class TestRelation():
    c = 1
    r = 2
    t = 3
    theta = 4
    lambda_ = 5
    R = Relation(c, r, t, theta, lambda_, nomask=True)

    def test_Fa(self):
        with pytest.raises(AssertionError):
            d = 5
            self.R._Fa(self.t - 1, d)
        fa = self.R._Fa(5, 100)
        assert fa == -(5 - self.R.t)

        fa = self.R._Fa(6, 1)
        assert fa == -(6 - self.R.t) / 100 - log(100)

        fa = self.R._Fa(10, 2)
        assert fa == -2 * (10 - self.R.t) / 100 - log(100 / 2)

        self.R.first_deg_adj = True
        fa = self.R._Fa(99, 2)
        # expect = log(99 - self.R.t) - log(factorial(1)) -
        #          2 * (99 - self.R.t) / 100 - 2 *log(100 / 2)
        expect = -5.595170185988091
        assert fa == expect
        self.R.first_deg_adj = False

    def test_Sa(self):
        d = 4
        sa = self.R._Sa([5], d)
        fa = self.R._Fa(5, d)
        assert sa == fa

        sa = self.R._Sa([5, 10], d)
        fa1 = self.R._Fa(5, d)
        fa2 = self.R._Fa(10, d)
        fa = fa1 + fa2
        assert sa == fa

    def test_p(self):
        assert self.R._p(100) == exp(-self.R.t)

    def test_Na(self):
        for n in range(5):
            for d in range(1, 5):
                if self.R.first_deg_adj and d == 2:
                    lambda_ = (3/4) * self.R.c + 2 * d * self.R.r * (3/4) * (1/4)
                else:
                    lambda_ = (self.R.a * (self.r * d + self.c) * self.R._p(d)) / (2 ** (d - 1))
                Na_obs = self.R._Na(n, d)
                Na_exp = log(poisson.pmf(n, lambda_))
                assert Na_obs == Na_exp

    def test_LLr(self):
        s = sorted([10, 8, 6, 4, 3])
        n = len(s)
        d = 3
        for np in range(n + 1):
            na = n - np
            mlr_obs = self.R._LLr(np, na, s, d)
            mlr_exp = self.R._Np(np) + self.R._Na(na, d) + self.R._Sp(s[:np])
            sa = self.R._Sa(s[np:], d)
            mlr_exp += sa
            assert mlr_obs == mlr_exp

    def test_MLL(self):
        self.R.first_deg_adj = True
        max_np, max_mll = self.R.MLL(1, [4], 2)
        assert max_np == 1
        assert max_mll == -7.27685644868579
        self.R.first_deg_adj = False

        max_np, max_mll = self.R.MLL(5, [10, 5, 3], 8)
        assert max_np == 5
        assert max_mll == -8.35813328956702


class TestRelationAvuncular():
    c = 1
    r = 2
    t = 3
    theta = 4
    lambda_ = 5
    R = Relation(c, r, t, theta, lambda_, nomask=True, avuncular_adj=True)

    def test_Na(self):
        d = 3
        for n in range(5):
            lambda_ = (3/4) * (self.r + self.c)
            Na_obs = self.R._Na(n, d)
            Na_exp = log(poisson.pmf(n, lambda_))
            assert Na_obs == Na_exp


def test_estimate_relation():
    """
    Uses hypothetical data to check:

    Pair            LLn         LLr         Reject  d   low_d   upp_d
    TestA:TestB     -78.0734    -30.5848    True    7   6       9
    TestB:TestC     -78.0734    -30.5848    True    7   7       9
    """

    MAX_D = 10

    t = 2.5                 # in cM
    h = 10                  # in cM
    theta = 3.197036753     # in cM
    lambda_ = 13.73         #
    r = 35.2548101          # ~for humans
    c = 22                  # human autosomes
    alpha = 0.05

    pair_dict = get_pair_dict('ersa/tests/test_data/test_LL.match', t, nomask=True)
    h0 = Background(t, theta, lambda_)
    ha = Relation(c, r, t, theta, lambda_, nomask=True)
    dob = (None, None)
    for pair, seg_list in pair_dict.items():
        s = [seg.length for seg in seg_list]
        n = len(s)
        est = estimate_relation(pair, dob, n, s, h0, ha, MAX_D, alpha, True)
        if pair == 'TestA:TestB':
            assert est.null_LL == -28.77225606065412
            assert est.max_LL == -26.926635378673502
            assert not est.reject
            assert est.d == 7
            # assert est.lower_d == 6
            # assert est.upper_d == 9
        if pair == 'TestB:TestC':
            assert est.null_LL == -16.66416439904802
            assert est.max_LL == -16.88720766845156
            assert not est.reject
            assert est.d == 9
            # assert est.lower_d == 7
            # assert est.upper_d == 9


def test_potential_relationship():
    indv1 = "A"
    indv2 = "B"

    # rel_est = potential_relationship(1, indv1, indv2, 1957, 1991)
    # assert rel_est == ("Child", "Parent")

    rel_est = potential_relationship(2, indv1, indv2, 1998, 1991)
    assert rel_est == ("Sibling", "Sibling")

    rel_est = potential_relationship(2, indv1, indv2, 1998, 1940)
    assert rel_est == ("Grandparent", "Grandchild")

    rel_est = potential_relationship(3, indv1, indv2, 1998, 1940)
    assert rel_est is None

    rel_est = potential_relationship(4, indv1, indv2, 1998, 1940)
    assert rel_est == ("Great Aunt/Uncle", "Great Niece/Nephew")

    rel_est = potential_relationship(4, indv1, indv2, 1938, 1940)
    assert rel_est == ("1st Cousin", "1st Cousin")

    rel_est = potential_relationship(7, indv1, indv2, 1900, 1975)
    assert rel_est == ("1st Cousin Thrice Removed", "1st Cousin Thrice Removed")

    rel_est = potential_relationship(5, indv1, indv2, 1900, 2035)
    assert rel_est == ("3rd Great Grandchild", "3rd Great Grandparent")


def test_n_to_ord():
    assert _n_to_ord(1) == "1st"
    assert _n_to_ord(2) == "2nd"
    assert _n_to_ord(3) == "3rd"
    assert _n_to_ord(4) == "4th"
    assert _n_to_ord(22) == "22nd"
    assert _n_to_ord(45) == "45th"
    assert _n_to_ord(101) == "101st"

def test_n_to_w():
    assert _n_to_w(1) == "Once"
    assert _n_to_w(2) == "Twice"
    assert _n_to_w(3) == "Thrice"
    assert _n_to_w(4) == "Four"
    assert _n_to_w(7) == "Seven"
    assert _n_to_w(10) == "Ten"
    assert _n_to_w(10, capitalize=False) == "ten"
