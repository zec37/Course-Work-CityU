function [ signal ] = Ham_win( signal )
%perform hamming window.
w = size(signal, 1);
for i = 1:w
    signal(i) = signal(i) * (0.54 - 0.46 * cos(2 * pi * i / (w - 1)));
end

end

