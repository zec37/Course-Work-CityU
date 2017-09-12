function [ comp, bit ] = rice( signalin, w )
%UNTITLED8 此处显示有关此函数的摘要
%   此处显示详细说明
N = size(data, 1);
signalin = reshape(signalin, [w, (N / w)]);

normal = data;

for i = 1:N
    if data(i) < 0
        normal(i) = - data(i) * 2;
    else
        normal(i) = data(i) * 2 - 1;
    end
end

ref = mode(normal);
k = 0;
while 2 ^ k <= ref
    k = k + 1;
end
k = k - 1;
M = 2 ^ k;

comp = cell(N, 1);
for i = 1:N
    q = fix(normal(i) / M);
    r = normal(i) - q * M;
    num_ini = [];
    if q > 0
        for j = 1:q
            num_ini = [num_ini 1];
        end
    end
    remain = dec2bin(r);
    num_rem = remain-'0';
    comp{i} = [num_ini 0 num_rem];
end
[~, bit_total] = cellfun(@size, comp);
bit = mean(bit_total);


end

