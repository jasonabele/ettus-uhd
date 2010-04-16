//
// Copyright 2010 Ettus Research LLC
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include <uhd/usrp/subdev_props.hpp>
#include <uhd/types/ranges.hpp>
#include <uhd/utils/assert.hpp>
#include <uhd/utils/static.hpp>
#include <uhd/usrp/dboard_base.hpp>
#include <uhd/usrp/dboard_manager.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/format.hpp>

using namespace uhd;
using namespace uhd::usrp;
using namespace boost::assign;

/***********************************************************************
 * The basic and lf boards:
 *   They share a common class because only the frequency bounds differ.
 **********************************************************************/
class basic_rx : public rx_dboard_base{
public:
    basic_rx(ctor_args_t const& args, double max_freq);
    ~basic_rx(void);

    void rx_get(const wax::obj &key, wax::obj &val);
    void rx_set(const wax::obj &key, const wax::obj &val);

private:
    double _max_freq;
};

class basic_tx : public tx_dboard_base{
public:
    basic_tx(ctor_args_t const& args, double max_freq);
    ~basic_tx(void);

    void tx_get(const wax::obj &key, wax::obj &val);
    void tx_set(const wax::obj &key, const wax::obj &val);

private:
    double _max_freq;
};

/***********************************************************************
 * Register the basic and LF dboards
 **********************************************************************/
static dboard_base::sptr make_basic_rx(dboard_base::ctor_args_t const& args){
    return dboard_base::sptr(new basic_rx(args, 90e9));
}

static dboard_base::sptr make_basic_tx(dboard_base::ctor_args_t const& args){
    return dboard_base::sptr(new basic_tx(args, 90e9));
}

static dboard_base::sptr make_lf_rx(dboard_base::ctor_args_t const& args){
    return dboard_base::sptr(new basic_rx(args, 32e6));
}

static dboard_base::sptr make_lf_tx(dboard_base::ctor_args_t const& args){
    return dboard_base::sptr(new basic_tx(args, 32e6));
}

UHD_STATIC_BLOCK(reg_basic_and_lf_dboards){
    dboard_manager::register_dboard(0x0000, &make_basic_tx, "Basic TX");
    dboard_manager::register_dboard(0x0001, &make_basic_rx, "Basic RX", list_of("AB")("A")("B"));
    dboard_manager::register_dboard(0x000e, &make_lf_tx,    "LF TX");
    dboard_manager::register_dboard(0x000f, &make_lf_rx,    "LF RX",    list_of("AB")("A")("B"));
}

/***********************************************************************
 * Basic and LF RX dboard
 **********************************************************************/
basic_rx::basic_rx(ctor_args_t const& args, double max_freq) : rx_dboard_base(args){
    _max_freq = max_freq;
}

basic_rx::~basic_rx(void){
    /* NOP */
}

void basic_rx::rx_get(const wax::obj &key_, wax::obj &val){
    wax::obj key; std::string name;
    boost::tie(key, name) = extract_named_prop(key_);

    //handle the get request conditioned on the key
    switch(key.as<subdev_prop_t>()){
    case SUBDEV_PROP_NAME:
        val = std::string(str(boost::format("%s - %s")
            % dboard_id::to_string(get_rx_id())
            % get_subdev_name()
        ));
        return;

    case SUBDEV_PROP_OTHERS:
        val = prop_names_t(); //empty
        return;

    case SUBDEV_PROP_GAIN:
        val = float(0);
        return;

    case SUBDEV_PROP_GAIN_RANGE:
        val = gain_range_t(0, 0, 0);
        return;

    case SUBDEV_PROP_GAIN_NAMES:
        val = prop_names_t(); //empty
        return;

    case SUBDEV_PROP_FREQ:
        val = double(0);
        return;

    case SUBDEV_PROP_FREQ_RANGE:
        val = freq_range_t(+_max_freq, -_max_freq);
        return;

    case SUBDEV_PROP_ANTENNA:
        val = std::string("");
        return;

    case SUBDEV_PROP_ANTENNA_NAMES:
        val = prop_names_t(1, ""); //vector of 1 empty string
        return;

    case SUBDEV_PROP_QUADRATURE:
        val = (get_subdev_name() == "AB"); //only quadrature in ab mode
        return;

    case SUBDEV_PROP_IQ_SWAPPED:
        val = false;
        return;

    case SUBDEV_PROP_SPECTRUM_INVERTED:
        val = false;
        return;

    case SUBDEV_PROP_USE_LO_OFFSET:
        val = false;
        return;
    }
}

void basic_rx::rx_set(const wax::obj &key_, const wax::obj &val){
    wax::obj key; std::string name;
    boost::tie(key, name) = extract_named_prop(key_);

    //handle the get request conditioned on the key
    switch(key.as<subdev_prop_t>()){

    case SUBDEV_PROP_GAIN:
        ASSERT_THROW(val.as<float>() == float(0));
        return;

    case SUBDEV_PROP_ANTENNA:
        ASSERT_THROW(val.as<std::string>() == std::string(""));
        return;

    case SUBDEV_PROP_FREQ:
        return; // it wont do you much good, but you can set it

    default: throw std::runtime_error(str(boost::format(
            "Error: trying to set read-only property on %s subdev"
        ) % dboard_id::to_string(get_rx_id())));
    }
}

/***********************************************************************
 * Basic and LF TX dboard
 **********************************************************************/
basic_tx::basic_tx(ctor_args_t const& args, double max_freq) : tx_dboard_base(args){
    _max_freq = max_freq;
}

basic_tx::~basic_tx(void){
    /* NOP */
}

void basic_tx::tx_get(const wax::obj &key_, wax::obj &val){
    wax::obj key; std::string name;
    boost::tie(key, name) = extract_named_prop(key_);

    //handle the get request conditioned on the key
    switch(key.as<subdev_prop_t>()){
    case SUBDEV_PROP_NAME:
        val = dboard_id::to_string(get_tx_id());
        return;

    case SUBDEV_PROP_OTHERS:
        val = prop_names_t(); //empty
        return;

    case SUBDEV_PROP_GAIN:
        val = float(0);
        return;

    case SUBDEV_PROP_GAIN_RANGE:
        val = gain_range_t(0, 0, 0);
        return;

    case SUBDEV_PROP_GAIN_NAMES:
        val = prop_names_t(); //empty
        return;

    case SUBDEV_PROP_FREQ:
        val = double(0);
        return;

    case SUBDEV_PROP_FREQ_RANGE:
        val = freq_range_t(+_max_freq, -_max_freq);
        return;

    case SUBDEV_PROP_ANTENNA:
        val = std::string("");
        return;

    case SUBDEV_PROP_ANTENNA_NAMES:
        val = prop_names_t(1, ""); //vector of 1 empty string
        return;

    case SUBDEV_PROP_QUADRATURE:
        val = true;
        return;

    case SUBDEV_PROP_IQ_SWAPPED:
        val = false;
        return;

    case SUBDEV_PROP_SPECTRUM_INVERTED:
        val = false;
        return;

    case SUBDEV_PROP_USE_LO_OFFSET:
        val = false;
        return;
    }
}

void basic_tx::tx_set(const wax::obj &key_, const wax::obj &val){
    wax::obj key; std::string name;
    boost::tie(key, name) = extract_named_prop(key_);

    //handle the get request conditioned on the key
    switch(key.as<subdev_prop_t>()){

    case SUBDEV_PROP_GAIN:
        ASSERT_THROW(val.as<float>() == float(0));
        return;

    case SUBDEV_PROP_ANTENNA:
        ASSERT_THROW(val.as<std::string>() == std::string(""));
        return;

    case SUBDEV_PROP_FREQ:
        return; // it wont do you much good, but you can set it

    default: throw std::runtime_error(str(boost::format(
            "Error: trying to set read-only property on %s subdev"
        ) % dboard_id::to_string(get_tx_id())));
    }
}