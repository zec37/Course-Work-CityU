function [ a ] = CalA( r )
%calculate predicted coefficients.
p = size(r, 1) - 1;
rp = r(2:p+1);
ri = r(1:p);
rf = flipud(ri);
rf = rf(2:p);
R = zeros(p, p);
temp = [ri;rf];
for i = 1:p
    temp_c = circshift(temp, i - 1);
    R(:, i) = temp_c(1:p);
end
a = -inv(R) * rp;

end

