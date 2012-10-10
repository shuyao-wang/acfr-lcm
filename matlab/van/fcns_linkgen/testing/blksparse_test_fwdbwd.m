% make a tridiagonal matrix with an off-diagonal element at ii,jj
clc;
N  = 9;
ii = 2;
jj = 7;
pdtest = 1;
while pdtest > 0
  Lambda = randcov(N);
  Lij = Lambda(ii,jj);
  Lji = Lambda(jj,ii);
  Lambda = tril(triu(Lambda,-1),1);
  Lambda(ii,jj) = Lij;
  Lambda(jj,ii) = Lji;
  [R,pdtest] = chol(Lambda);
end

Lambda_T = Lambda;
Lambda_Tprev = Lambda_T;
xt     = ii+1:N
m0     = ii
mplus  = markov_blanket(Lambda_T,xt);
mplus  = setdiff(mplus,m0)
mminus = 1:N;
mminus([xt,m0,mplus]) = []

[junk,Lambda_T,Lambda_U,Lambda_V,Lambda_D] = sparsify_ryan(zeros(N,1),Lambda_T,xt,m0,mplus,mminus);
Lambda_Tprev
Lambda_T
change = Lambda_T-Lambda_Tprev
disp('---------------------------------------------------------------');
  

