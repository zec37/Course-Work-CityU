%function [ en, a_matrix, a_int, comp_en, bit_en, comp_a, bit_a ] = monocoder( filename )
%UNTITLED 此处显示有关此函数的摘要
%   linear prediction for monocoder

signal = audioread('C:\Users\My\OneDrive\文档\CourseFolder-EE5809-digital audio\assignment\music_source.wav');
signal_int = int16(signal * 32768);
W = size(signal,1);
w = 512;
N = 256;
P = 16;
f = fix(W / N);
a_matrix = zeros(P, f - 2);
xn = zeros(N, f - 2);

for i = 1:f - 2
    buf = signal((i - 1) * N + 1: (i - 1) * N + w);
    buf = Ham_win(buf);
    r = calR(buf, P);
    a = CalA(r);
    a_matrix(:, i) = a;
    pre = Li_Pre(buf, a, N);
    xn(:, i) = pre;
end

pre_x = reshape(xn, [], 1);
%pre_x_int = int16(pre_x * 32768);
en = pre_x + signal(N + 1: N + N * (f - 2));
en_int = int16(en * 32768);
a_int = int16(a_matrix * 128);
% [ comp_en, bit_en ] = rice( en );
% [ comp_a, bit_a ] = rice( a_int );

%end

% function [ buf ] = Ham_win( buf )
% %perform hamming window.
% win_size = size(buf, 1);
% for i = 1:win_size
%     win = 0.54 - 0.46 * cos(2 * pi * (i - 1) / win_size);
% end
% buf = buf .* win;
% 
% end
% 
% function [ r ] = calR( buf, p )
% %calculate autocorrelation.
% w = size(buf, 1);
% r = zeros(p + 1,1);
% for i = 1:p+1
%     signal_n = buf(1:w + 1 - i);
%     signal_i = buf(i:w);
%     product = signal_n .* signal_i;
%     r(i) = sum(product);
% end
% 
% end
% 
% 
% %function [ a ] = CalA( r )
% %calculate predicted coefficients.
% p = size(r, 1) - 1;
% rp = r(2:p+1);
% ri = r(1:p);
% R = zeros(p, p);
% temp = [ri;flipud(ri)];
% for i = 1:p
%     temp_c = circshift(temp, i - 1);
%     R(:, i) = temp_c(1:p);
% end
% a = -(R ^ -1) * rp;
% 
% end
% 
% function [ pre ] = Li_Pre( signal, a, n )
% %Calculate predicted signal.
% w = size(signal, 1);
% p = size(a, 1);
% pre = zeros(n,1);
% ini = (w - n) / 2;
% for i = 1:n
%     fore = signal(ini + i - p:ini + i - 1);
%     pre(i) = a' * fore; 
% end
% 
% end
% 
% function [ comp, bit ] = rice( data )
% %UNTITLED8 此处显示有关此函数的摘要
% %   此处显示详细说明
% N = size(data, 1);
% normal = data;
% 
% for i = 1:N
%     if data(i) < 0
%         normal(i) = - data(i) * 2;
%     else
%         normal(i) = data(i) * 2 - 1;
%     end
% end
% 
% ref = mode(normal);
% k = 0;
% while 2 ^ k <= ref
%     k = k + 1;
% end
% k = k - 1;
% M = 2 ^ k;
% 
% comp = cell(N, 1);
% for i = 1:N
%     q = fix(normal(i) / M);
%     num_ini = [];
%     if q > 0
%         for j = 1:q
%             num_ini = [num_ini 1];
%         end
%     end
%     r = normal(i) - q * M;
%     remain = dec2bin(r);
%     num_rem = remain-'0';
%     comp{i} = [num_ini 0 num_rem];
% end
% [~, bit_total] = cellfun(@size, comp);
% bit = mean(bit_total);
% 
% end
