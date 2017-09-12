function [ r ] = calR( signal, p )
%calculate autocorrelation.
w = size(signal, 1);
r = zeros(p + 1,1);
for i = 1:p+1
    signal_n = signal(1:w + 1 - i);
    signal_i = signal(i:w);
    product = signal_n .* signal_i;
    r(i) = sum(product);
end

end

