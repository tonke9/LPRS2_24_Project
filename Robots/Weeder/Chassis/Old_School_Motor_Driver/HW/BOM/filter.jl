#!/usr/bin/env julia

using DataFrames
using CSV
using Query


t_in = CSV.read(
	"Sprint_MOSFETs.tsv",
	copycols = true,
	delim = '\t'
)

t_on_stock = @from r in t_in begin
	@where r[:Status] == "on stock"
	@select r
	@collect DataFrame
end

t_filt = t_on_stock |> @select(
	:Name,
	:Price,
	:Type,
	:Package,
	:Current,
	:Voltage,
	:Resistance,
	:Power,
) |> DataFrame

t_work = deepcopy(t_filt)

t_I_and_V = @from r in t_work begin
	@where r[:Current] > 12 && r[:Voltage] < 200
	@select r
	@collect DataFrame
end

#@show minimum(t_I_and_V[:Voltage])

all_packages = [
	pack for pack in sort(unique(t_work[:Package])) if !ismissing(pack)
]
allowable_packages = [
	pack for pack in all_packages
		if any(
			s -> startswith(pack, s),
			["TO-220"]
			#["TO-220", "TO-247"]
		)
]
t_good_package = @from r in t_I_and_V begin
	@where r[:Package] in allowable_packages
	@select r
	@collect DataFrame
end

t_price = @from r in t_good_package begin
	@where r[:Price] < 100
	@select r
	@collect DataFrame
end

t_final = t_price

t_p_mosfets = @from r in t_final begin
	@where r[:Type] == "P-MOSFET"
	@select r
	@collect DataFrame
end

t_n_mosfets = @from r in t_final begin
	@where r[:Type] == "N-MOSFET"
	@select r
	@collect DataFrame
end

CSV.write("filtered.tsv", t_final, delim = '\t')


println("End")
