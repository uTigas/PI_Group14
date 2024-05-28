import { IonAlert, IonButton, IonCard, IonCardContent, IonCardHeader, IonCardSubtitle, IonCardTitle, IonCol, IonContent, IonGrid, IonHeader, IonIcon, IonInput, IonItem, IonItemDivider, IonLabel, IonList, IonListHeader, IonModal, IonPage, IonPopover, IonRow, IonSearchbar, IonText, IonTitle, SearchbarInputEventDetail } from "@ionic/react"
import { useParams } from "react-router";
import "../support/General.css";
import { addOutline, arrowBack, arrowDown, arrowUpOutline, cloud, cloudDownloadOutline, cloudOutline, createOutline, download, fileTray, fileTrayOutline, trashBin, trashBinOutline } from "ionicons/icons";
import UploadComponent from "../components/UploadComponent";
import { useContext, useEffect, useRef, useState } from "react";
import ApiWrapper from "../support/APIWrapper";
import { IonSearchbarCustomEvent } from '@ionic/core';
import { UserContext } from "../App";
import { format } from "date-fns";
import Common from "../support/Common";
import AppAppBar from "../components/AppAppBar";
import { OverlayEventDetail } from "@ionic/react/dist/types/components/react-component-lib/interfaces";

const OrganizationVault: React.FC = () => {
    const { id: vaultId } = useParams<{ id: string }>();
    const [items, setItems] = useState<any[]>([]);
    const [organizations, setOrganizations] = useState<any[]>([]);
    const [members, setMembers] = useState<any[]>([]);
    const [title, setTitle] = useState<string>("");
    const [fileName, setFileName] = useState<string>("");
    const modal = useRef<HTMLIonModalElement>(null);

    const userDetails = useContext(UserContext);
    const fetchOrganizationVaultDetails = async () => {
        try {
            const response = await ApiWrapper.fetchOrganizationVaultDetails(vaultId);
            if (response) {
                console.log(response);
                setOrganizations(response.data.organizations);
                setMembers(response.data.members);
                setItems(response.data.items);
                setResults(response.data.items);
            }
        } catch (error) {

        }
    }

    function onWillDismiss(ev: CustomEvent<OverlayEventDetail>) { }

    const downloadFile = async (id: string, name: string, type: string,) => {
        try {
            const response = await ApiWrapper.downloadItem(id)
            const blob = new Blob([response!.data], { type: response!.headers['content-type'] });

            const url = window.URL.createObjectURL(blob);

            const link = document.createElement('a');
            link.href = url;
            link.setAttribute('download', `${name}.${type}`);
            document.body.appendChild(link);
            link.click();

            window.URL.revokeObjectURL(url);
            document.body.removeChild(link);

        } catch (error) {
            console.error('Error downloading file:', error);
        }
    }

    const [currentPage, setCurrentPage] = useState(1);
    const [itemsPerPage] = useState(10);
    const [results, setResults] = useState<any[]>([])

    function handleItemInput(ev: IonSearchbarCustomEvent<SearchbarInputEventDetail>): void {
        const target = ev.target as HTMLIonSearchbarElement;
        const query = target ? target.value!.toLowerCase() : '';
        setResults(items.filter((d) => d.name.toLowerCase().indexOf(query) > -1));
        setCurrentPage(1);
    }

    const handlePageChange = (newPage: number) => {
        setCurrentPage(newPage);
    };

    const paginatedResults = results.slice((currentPage - 1) * itemsPerPage, currentPage * itemsPerPage);

    const renderPagination = () => {
        const totalPages = Math.ceil(results.length / itemsPerPage);
        const pages = [];
        for (let i = 1; i <= totalPages; i++) {
            pages.push(
                <a key={i} className={i == currentPage ? 'active' : ''} onClick={() => handlePageChange(i)}>{i}</a>
            );
        }
        return pages;
    };

    const deleteItem = (id: string) => {
        const formData = new FormData;
        formData.append("id", id);
        ApiWrapper.deleteItem(formData)
        fetchOrganizationVaultDetails();
    }

    const renameItem = async (id: string) => {
        try {

            const formData = new FormData;
            formData.append("id", id);
            formData.append("name", fileName);
            const response = await ApiWrapper.renameItem(formData);
            fetchOrganizationVaultDetails();

            if (response) {
                setFileName('')
            }
            else {
                setFileName('')
            }
        } catch (error) {
            console.error('Error renaming file', error);
        }
    }

    useEffect(() => {
        fetchOrganizationVaultDetails();
    }, [])

    return (
        <IonPage>
            <IonHeader>
                <AppAppBar title={"Organization/Vault"} />
            </IonHeader>
            <IonContent>
                <IonGrid>
                    <IonRow>
                        <IonCol size="auto">
                            <IonButton className="return_button" onClick={() => history.back()} fill="outline" size="small" color={"secondary"}>
                                <IonIcon icon={arrowBack} size="large"></IonIcon>
                                Return
                            </IonButton>
                        </IonCol>
                    </IonRow>
                    <IonRow>
                        <IonCol>
                            <IonCard>
                                <IonCardContent>
                                    <IonGrid>
                                        <IonTitle>Items</IonTitle>
                                        <IonRow>
                                            <IonCol>
                                                <IonSearchbar mode="ios" animated={true} color='' placeholder='Search for a specific Item...' onIonInput={(ev) => handleItemInput(ev)}></IonSearchbar>
                                            </IonCol>
                                            <IonCol size="auto">
                                                <IonModal ref={modal} trigger="click-trigger-org" onWillDismiss={(ev) => onWillDismiss(ev)}>
                                                    <UploadComponent refModal={modal} vaultId={vaultId} user={""} />
                                                </IonModal>
                                                <IonButton className="create-org" color={'success'} fill='outline' shape="round" id="click-trigger-org">New<IonIcon icon={addOutline} /></IonButton>
                                            </IonCol>

                                        </IonRow>
                                        <IonRow>
                                            <IonCol><IonLabel><h2>Name</h2></IonLabel></IonCol>
                                            <IonCol><IonLabel><h2>Size</h2></IonLabel></IonCol>
                                            <IonCol><IonLabel><h2>Type</h2></IonLabel></IonCol>
                                            <IonCol><IonLabel><h2>Creation</h2></IonLabel></IonCol>
                                            <IonCol></IonCol>
                                        </IonRow>
                                        {paginatedResults.length !== 0 ? (
                                            paginatedResults.map((item) => (
                                                <div key={item.id}>
                                                    <IonItemDivider>
                                                    </IonItemDivider>
                                                    <IonRow>
                                                        <IonCol className='appt_col'>
                                                            <IonLabel>{item.name}</IonLabel>
                                                        </IonCol>
                                                        <IonCol className='appt_col'>
                                                            <IonLabel>{item.size / 1000 / 1024 > 1000 ? (item.size / 1000 / 1024 / 1024).toFixed(2) + " GB" : item.size / 1000 > 1024 ? (item.size / 1000 / 1024).toFixed(2) + " MB" : (item.size / 1000).toFixed(2) + " KB"} </IonLabel>
                                                        </IonCol>
                                                        <IonCol className='appt_col'>
                                                            <IonLabel>{item.type}</IonLabel>
                                                        </IonCol>
                                                        <IonCol className='appt_col'>
                                                            <IonLabel>{format(item.createdAt, "dd-MM-yyyy HH:mm")}</IonLabel>
                                                        </IonCol>
                                                        <IonCol>
                                                            <div className='appt_button'>
                                                                <IonButton onClick={() => downloadFile(item.id, item.name, item.type)} id={"download-" + item.id} shape='round' fill='outline' color={'primary'} size='small'><IonIcon size="medium" icon={cloudDownloadOutline} /></IonButton>
                                                                <IonButton id={"rename-" + item.id} shape='round' fill='outline' color={'success'} size='small'><IonIcon size="medium" icon={createOutline} /></IonButton>
                                                                <IonButton id={"delete-" + item.id} shape='round' fill='outline' color={'danger'} size='small'><IonIcon size="medium" icon={trashBinOutline} /></IonButton>
                                                                <IonAlert
                                                                    trigger={"delete-" + item.id}
                                                                    trigger-action="click"
                                                                    header="Delete File"
                                                                    subHeader="This is irreversible!"
                                                                    message="Do you wish to confirm file deletion?"
                                                                    buttons={[
                                                                        {
                                                                            text: 'YES',
                                                                            handler: () => {
                                                                                deleteItem(item.id);
                                                                            }
                                                                        },
                                                                        {
                                                                            text: 'NO',
                                                                            role: 'cancel',
                                                                            handler: () => {
                                                                            }
                                                                        },
                                                                    ]}
                                                                ></IonAlert>
                                                                <IonPopover
                                                                    trigger={"rename-" + item.id}
                                                                    trigger-action="click"

                                                                >
                                                                    <IonItem>
                                                                        <IonInput placeholder='Insert file name' onIonChange={(e) => { if (e.detail.value) setFileName(e.detail.value) }}></IonInput>
                                                                    </IonItem>
                                                                    <IonItem>
                                                                        <IonButton color={'success'} fill='outline' onClick={() => { if (fileName != '') renameItem(item.id) }}>Rename</IonButton>
                                                                    </IonItem>
                                                                </IonPopover>

                                                            </div>
                                                        </IonCol>
                                                    </IonRow>
                                                </div>
                                            ))
                                        ) : (
                                            <IonText>No Items stored yet. Start uploading! </IonText>
                                        )}
                                        <IonRow>
                                            {results.length > 0 ? (
                                                <div className="pagination ion-margin-top">
                                                    <a onClick={() => { currentPage > 1 ? setCurrentPage(currentPage - 1) : false }}>&laquo;</a>
                                                    {renderPagination()}
                                                    <a onClick={() => { currentPage < Math.ceil(results.length / itemsPerPage) ? setCurrentPage(currentPage + 1) : false }}>&raquo;</a>
                                                </div>
                                            ) : (<></>)}

                                        </IonRow>
                                    </IonGrid>
                                </IonCardContent>
                            </IonCard>
                        </IonCol>
                    </IonRow>
                </IonGrid>
            </IonContent>
        </IonPage>
    )
}
export default OrganizationVault;