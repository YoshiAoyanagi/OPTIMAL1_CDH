// 
// 
// 

#include "AD_SRX.h"


void	AD_SRX_init(AD_SRX_STS_STRUCT *srx)
{
	float tmp_mv;

	pinMode(PORT_AD_SRX_C_LOCK, INPUT);
	pinMode(PORT_AD_SRX_SC_LOCK, INPUT);
	pinMode(PORT_AD_SRX_RX_ON, OUTPUT);

	AD_SRX_RX_ON();

	srx->carrier_lock = digitalRead(PORT_AD_SRX_C_LOCK);
	srx->sub_carrier_lock = digitalRead(PORT_AD_SRX_SC_LOCK);

	AD_SRX_rec(srx);
}

void	AD_SRX_RX_ON(void)
{
	pinMode(PORT_AD_SRX_RX_ON, OUTPUT);
	digitalWrite(PORT_AD_SRX_RX_ON, LOW);
}

void	AD_SRX_RX_OFF(void)
{
	pinMode(PORT_AD_SRX_RX_ON, OUTPUT);
	digitalWrite(PORT_AD_SRX_RX_ON, HIGH);
}

void	AD_SRX_RX_RESET(void)
{
	AD_SRX_RX_OFF();
	delay(1000);
	AD_SRX_RX_ON();
}

void	AD_SRX_rec(AD_SRX_STS_STRUCT *srx)
{
	float tmp_mv;

	srx->carrier_lock = digitalRead(PORT_AD_SRX_C_LOCK);
	srx->sub_carrier_lock = digitalRead(PORT_AD_SRX_SC_LOCK);

	tmp_mv = analogRead(PORT_AN_AD_SRX_AGC_INC);
	tmp_mv = tmp_mv / 1023.0 * 2 * 2.048;

	srx->agc_inc = 15.493423 * (tmp_mv * tmp_mv * tmp_mv) - 89.675347 * (tmp_mv * tmp_mv) + 203.208504 * tmp_mv - 251.095251;

	tmp_mv = analogRead(PORT_AN_AD_SRX_AGC_COH);
	tmp_mv = tmp_mv / 1023.0 * 2.048;
	srx->agc_coh = 28.24014 * (tmp_mv * tmp_mv) - 15.71714 * tmp_mv - 126.71260;

	tmp_mv = analogRead(PORT_AN_AD_SRX_FREQ_ERR);
	tmp_mv = tmp_mv / 1023.0 * 3 * 2.048;
	srx->freq_err = 47.934 * tmp_mv - 112.48;
}
