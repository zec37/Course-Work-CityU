function [ pre ] = Li_Pre( signal, a, n )
%Calculate predicted signal.
w = size(signal, 1);
p = size(a, 1);
pre = zeros(n,1);
ini = (w - n) / 2;
for i = 1:n
    f = signal((ini + i - p):(ini + i - 1));
    pre(i) = a' * f; 
end

end

