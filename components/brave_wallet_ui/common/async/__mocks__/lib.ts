// Copyright (c) 2022 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at https://mozilla.org/MPL/2.0/.

import {
  mockBasicAttentionToken //
} from '../../../stories/mock-data/mock-asset-options'
import { BraveWallet, GetBlockchainTokenInfoReturnInfo } from '../../../constants/types'
import { HardwareWalletConnectOpts } from '../../../components/desktop/popup-modals/add-account-modal/hardware-wallet-connect/types'

let mockedAllowance = '1000000000000000000' // 1 unit

export const getERC20Allowance = (
  _fromAddress: string,
  _selectedAccountAddress: string,
  allowanceTarget: string
) => new Promise<string>((resolve) => {
  resolve(allowanceTarget || mockedAllowance)
})

export const setERC20Allowance = (newAllowance: string) => {
  mockedAllowance = newAllowance
}

export const isStrongPassword = (value: string) => {
  return value.length >= 8 // is at least 8 characters
}

export const onConnectHardwareWallet = (opts: HardwareWalletConnectOpts): Promise<BraveWallet.HardwareWalletAccount[]> => {
  const makeDerivationPath = (index: number): string => `m/44'/60'/${index}'/0/0`

  return new Promise((resolve) => {
    resolve(Array.from({ length: opts.stopIndex - opts.startIndex }, (_, i) => ({
      coin: BraveWallet.CoinType.ETH,
      keyringId: BraveWallet.KeyringId.kDefault,
      address: '0xc02aaa39b223fe8d0a0e5c4f27ead9083c756cc2',
      derivationPath: makeDerivationPath(i + opts.startIndex),
      name: 'Ledger 1',
      hardwareVendor: 'Ledger',
      deviceId: 'device1',
      network: undefined
    })))
  })
}

export const getBalance = (): Promise<string> => {
  return new Promise(async (resolve) => {
    resolve('0')
  })
}

export const getBlockchainTokenInfo = async (
  contractAddress: string
): Promise<GetBlockchainTokenInfoReturnInfo> => {
  if (
    contractAddress.toLowerCase() ===
    mockBasicAttentionToken.contractAddress.toLowerCase()
  ) {
    return {
      token: mockBasicAttentionToken
    }
  }
  return {
    token: null
  }
}

export const translateToNftGateway = async (url: string | undefined) => {
  return url
}
