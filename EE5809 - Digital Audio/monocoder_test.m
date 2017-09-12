%function [ en, a_matrix, a_int ] = monocoder_test( filename )
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
    buf = signal(i * N + 1: i * N + w);
    buf_win = hamming(w) .* buf;       %For testing
    r = xcorr(buf, P);                 %For testing
    a = levinson(r, P - 1);            %For testing
    a = a';                            %For testing
    %a = a(2:P + 1);
    a_matrix(:, i) = a;
    pre = Li_Pre(buf_win, a, N);
    xn(:, i) = pre;
end
pre_x = reshape(xn, [], 1);
pre_x_int = int16(pre_x * 32768);
a_int = int16(a_matrix * 128);
en = pre_x_int + signal_int(N + 1: N + N * (f - 2));


%end

% function [ pre ] = Li_Pre( signal, a, n )
% %Calculate predicted signal.
% w = size(signal, 1);
% p = size(a, 1);
% pre = zeros(n,1);
% ini = (w - n) / 2;
% for i = 1:n
%     f = signal(ini + i - p:ini + i - 1);
%     pre(i) = a' * f; 
% end
% 
% end