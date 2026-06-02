//TODO: implement/complete this module
const H1 = "H1";
const H2 = "H2";
const H3 = "H3";
const H4 = "H4";
const H5 = "H5";
const L6 = "L6";
const L7 = "L7";
const L8 = "L8";
const L9 = "L9";
const L10 = "L10";
const WS = "WS";
const R1 = "R1";
const R2 = "R2";
const R3 = "R3";
const R4 = "R4";
const R5 = "R5";
const SC = "SC";

const REELSETS = {
    basespin: {
        BG_reelSet1 : 
        [
            [
                	H5, L7, L10, H5, L9, H2, L8, H4, L10, H1, L8, H5, L6, H3, L8, L6, H3, L9, WS, L7,					
                        H5, L8, H3, L6, H1, L7, L10, H4, L8, L9, H3, L7, L10, R1, L9, L10,									
            ],
            [
                    H2, L9, R2, L10, L6, WS, L9, L10, H4, L7, H1, L6, H3, L7, H4, R2, L8, H1, L10, H2,					
                        L7, H4, L6, H5, WS, L9, H2, L8, H1, L9, H4, L8, H5, L7, L10, H5,									
            ],
            [
                    WS, H4, L8, H5, L10, H1, L6, H3, L10, L9, R3, L10, H3, L9, H2, L6, H5, L8, L9, H5,					
                        L6, H3, L7, WS, L9, H4, L6, L9, H4, L8, H2, L7, L10, H5, L8,								
            ],
            [
                    H5, L6, H1, R4, L10, H2, L9, H4, L7, H5, L6, H2, L10, H3, WS, L7, H2, L9, WS, H4,					
                        L8, L7, H3, L10, WS, H3, L8, L7, H2, WS, L10, H5, L7, H4, L6, H1, L9,									
            ],
            [
                    H3, L10, H4, WS, L7, H3, L8, H1, L7, L10, H5, L9, H2, WS, L6, H1, L8, H5, L9, H1,					
                        L7, WS, L9, L8, H4, L10, H2, L6, L7, H3, L9, H1, L6, WS, H5, L9, H4, L10, H2, L6,					
                        R5, H3, L10, H4, L7, H3, L8, H1, L7, L10, H5, L9, H2, WS, L6, H1, L8, H5, L9, H1,					
                        L7, L9, H4, L10, H2, L6, WS, L7, H3, L9, H1, L6, WS, H5, L9, H4, L10, L8, H2, L6,					
                        R5,									
            ]
        ],
    },
    freespin: {
        FS_reelSet1 : [
            [
                    H4, L8, L9, H3, L7, L6, WS, L8, L7, H1, L6, L10, SC, H4, L8, H3, L7, H5, L10, L6,					
                        H5, L10, L9, H4, L10, L9, H2, L6,									
            ],
            [
                    L6, H3, L7, H1, H4, L8, L10, H1, L7, L6, H5, L9, H2, L8, WS, L9, H3, L8, H2, L7,					
                        H5, L9, L10, WS, H5, L6, H4, L10, H3, L7,									
            ],
            [
                    WS, L9, H5, L6, H3, L7, H4, L8, L6, H4, L8, WS, L7, H4, L10, H5, L8, H4, L10, H5,					
                        L8, H1, L6, L9, WS, L10, H5, L9, H3, L10, H2, L6, H5, L8, L9, H5, L6, L9, H4, L7,					
                        L8, H3, L6, H4, L8, WS, L7, H3, L10, H5, L8, H4, H5, L8, L10, H1, L6, H3, L10, H5,					
                        L9, H3, L10, H2, L6, H5, L8,									
            ],
            [
                    L6, H1, L10, L9, H2, H4, L8, L7, H1, H5, L6, H1, L9, H3, WS, L7, L9, H2, H4, L8,					
                        L7, H3, L9, WS, H3, L8, H4, L7, H2, L10, L8, WS, L7, H4, L6, L9, H5,									
            ],
            [
                    L8, L10, H2, L9, H5, L8, SC, L7, L9, H4, L10, L6, H4, L7, L9, H1, L6, H5, WS, L9,					
                        H4, L10, H2, L6, H4, L10, H5, SC, L7, H3, L8, H1, L7, H3, L10, H5,									
            ]
        ],

        FS_reelSet2: [
            [
                    H4, L8, L9, H3, L7, L6, WS, L8, L7, H1, L6, L10, SC, H4, L8, H3, L7, H5, L10, L6,					
                        H5, L10, L9, H4, L10, L9, H2, L6, H4, L8, L9, H3, L7, L6, WS, L8, L7, H1, L6, L10,					
                        SC, H4, L8, H3, L7, H5, L10, L6, H5, L10, L9, H4, L10, L9, H2, L6, H4, L8, L9, H3,					
                        L7, L6, WS, L8, L7, H1, L6, L10, SC, H4, L8, H3, L7, H5, L10, L6, H5, L10, SC, L9,					
                        H4, L10, L9, H2, L6,									
            ],
            [
                    L6, H3, L7, H1, H4, L8, L10, H1, L7, L6, H5, L9, H2, L8, WS, L9, H3, L8, H2, L7,					
                        H5, L9, L10, WS, H5, L6, H4, L10, H3, L7,									
            ],
            [
                    WS, L9, H5, L6, H3, L7, H4, L8, L6, H4, L8, WS, L7, H4, L10, H5, L8, H4, L10, H5,					
                        L8, H1, L6, L9, WS, L10, H5, L9, H3, L10, H2, L6, H5, L8, L9, H5, L6, L9, H4, L7,					
                        L8, H3, L6, H4, L8, WS, L7, H3, L10, H5, L8, H4, H5, L8, L10, H1, L6, H3, L10, H5,					
                        L9, H3, L10, H2, L6, H5, L8,									       
            ],
            [
                    L6, H1, L10, L9, H2, H4, L8, L7, H1, H5, L6, H1, L9, H3, WS, L7, L9, H2, H4, L8,					
                        L7, H3, L9, WS, H3, L8, H4, L7, H2, L10, L8, WS, L7, H4, L6, L9, H5,									       
            ],
            [
                    L8, L10, H2, L9, H5, L8, SC, L7, L9, H4, L10, L6, H4, L7, L9, H1, L6, H5, WS, L9,					
                        H4, L10, H2, L6, H4, L10, H5, SC, L7, H3, L8, H1, L7, H3, L10, H5, L8, L10, H2, L9,					
                        H5, L8, SC, L7, L9, H4, L10, L6, H4, L7, L9, H1, L6, H5, WS, L9, H4, L10, H2, L6,					
                        H4, L10, H5, SC, L7, H3, L8, H1, L7, H3, L10, H5, L8, L10, H2, L9, H5, L8, SC, L7,					
                        L9, H4, L10, L6, H4, L7, L9, H1, SC, L6, H5, WS, L9, H4, L10, H2, L6, H4, L10, H5,					
                        SC, L7, H3, L8, H1, L7, H3, L10, H5,									       
            ]
        ],
    }
}
